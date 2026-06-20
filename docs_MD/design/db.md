\page db The user database

We make use of files and folders to create our "database" (**the users/ folder**, if you haven't renamed it), which can be described in file_struct.yaml.txt:

**this is only intended to aid development and won't actually change where the program looks**
\n But you can freely change the content of \ref config "KV files", and generate_file_structure() will preserve existing values, while removing fields that aren't in content: .
  
./users contain folders of usernames, where each stores their own data

YAML format:
@code{.yaml}
Usertype:
  - path: "users/%s/" # This is a format string, %s is for the username
    type: directory
  
  - path: "users/%s/prefs.txt"
    type: KV file
    content:
      key: value
      blabla: blablabla

Usertype2:
  - path: "users/%s/"
    type: directory

  - path: "users/%s/temp.txt"
  	type: file
  	content:
  
...
@endcode

In JSON, objects are enclosed with curly braces, but for YAML its indentation.

The top level key, usertype, stores an object that is an array of objects, where each element is denoted as -

The objects in the array must start with path, followed by type, then the content.

So the parsing loop in \ref generate_file_structure() will need to:
-# Skip everything until we reach the top level key of the correct user type
-# Store the path, and remember that it will parse the type next
-# If the type is a directory, mkdir. Else create a FILE pointer. 
-# If we have a FILE pointer, start parsing the content object: 
	- if its just a file, dump everything in the content object
	- if its a KV file
		-# parse everthing in the content object as a KV file
		-# compare with the existing KV file
			- if a value exists for a key in the original file, preserve that
			- field is removed if the key doesnt exist in the content object, 
-# Stop parsing content object when reach the next "- path"
-# End loop at the next top level key


\dot
digraph GenerateFileStructure {
    // Graph settings
    rankdir=TB;
    node [shape=box, style=filled, fillcolor=white, fontname="Helvetica,Arial,sans-serif"];
    edge [fontname="Helvetica,Arial,sans-serif", fontsize=10];

    // Nodes
    Start [shape=oval, fillcolor=lightblue, label="Start"];
    OpenFile [label="Open file_struct.yaml.txt"];
    FindRole [label="Find top-level key for role"];
    CheckRoleFound [shape=diamond, label="Role found?", fillcolor=lightyellow];
    
    LoopStart [shape=point, width=0];
    ParseLine [label="Read next line"];
    CheckIndent [shape=diamond, label="Check Indentation", fillcolor=lightyellow];
    ResetState [label="Reset state:\nin_path=0, in_cont=0\nin_cont_KV=0\nClose out_file"];
    
    CheckLineType [shape=diamond, label="State & Line Type?", fillcolor=lightyellow];
    
    PathNode [label="Parse path string\nSet in_path=1\nSet in_type=0"];
    TypeNode [shape=diamond, label="Type?", fillcolor=lightyellow];
    
    DirNode [label="mkdir if not exists\nSet in_path=0"];
    FileNode [label="Open out_file if not exists\nSet in_cont=1"];
    KVNode [label="Open out_file\nParse existing KV if exists\nSet in_cont_KV=1 (if exists) or in_cont=1"];
    
    ContentFile [label="Dump line to out_file\n(if out_file && in_cont)"];
    ContentKV [label="Parse line as KV\nPreserve original value if exists\nWrite to out_file\n(if out_file && in_cont_KV)"];
    
    EndParsing [shape=oval, label="Success / End", fillcolor=lightgreen];
    ErrorExit [shape=oval, label="Exit with Error", fillcolor=lightpink];
    
    CloseFile [label="Close file_struct.yaml.txt if open"];

    // Edges
    Start -> OpenFile;
    OpenFile -> FindRole;
    FindRole -> CheckRoleFound;
    
    CheckRoleFound -> LoopStart [label="Yes"];
    CheckRoleFound -> ErrorExit [label="No"];
    
    LoopStart -> ParseLine;
    ParseLine -> CheckIndent;
    CheckIndent -> EndParsing [label="No indent (Next top-level key)"];
    CheckIndent -> ResetState [label="Indent < INDENT_SPACES"];
    CheckIndent -> CheckLineType [label="Indent >= INDENT_SPACES"];
    
    ResetState -> CheckLineType;
    
    CheckLineType -> LoopStart [label="Empty/Whitespace"];
    CheckLineType -> ContentFile [label="out_file && in_cont"];
    CheckLineType -> ContentKV [label="out_file && in_cont_KV"];
    CheckLineType -> PathNode [label="!in_path && '- path:'"];
    CheckLineType -> TypeNode [label="!in_type && 'type:'"];
    
    PathNode -> LoopStart;
    
    TypeNode -> DirNode [label="directory"];
    TypeNode -> FileNode [label="file"];
    TypeNode -> KVNode [label="KV file"];
    TypeNode -> ErrorExit [label="unknown"];
    
    DirNode -> LoopStart;
    FileNode -> LoopStart;
    KVNode -> LoopStart;
    
    ContentFile -> LoopStart;
    ContentKV -> LoopStart;
    
    EndParsing -> CloseFile;
    CloseFile -> End; // implicitly reaching end of function
}
\enddot


\ref regen_file_structure.c : 

\dot
digraph RegenFileStructure {
    // Graph settings
    rankdir=TB;
    node [shape=box, style=filled, fillcolor=white, fontname="Helvetica,Arial,sans-serif"];
    edge [fontname="Helvetica,Arial,sans-serif", fontsize=10];

    // Nodes
    Start [shape=oval, fillcolor=lightblue, label="Start"];
    ReadCreds [label="Get all usernames from creds file\n(users/creds.txt)"];
    UserLoop [shape=diamond, label="For each user?", fillcolor=lightyellow];
    
    ReadProfile [label="Read user profile\n(users/%s/profile.txt)"];
    ProfileExists [shape=diamond, label="Is profile empty\nor failed to open file?", fillcolor=lightyellow];
    
    CheckUsertype [shape=diamond, label="Has USERTYPE?", fillcolor=lightyellow];
    GenStructure [label="generate_file_structure(USERTYPE, USERNAME)", fillcolor=lightgreen];
    
    ErrorExit [shape=oval, label="Exit with Error\n(Missing USERTYPE)", fillcolor=lightpink];
    ErrorExit_2 [shape=oval, label="Exit with Error\n(Failed to open file)", fillcolor=lightpink];
    End [shape=oval, label="Success / End", fillcolor=lightgreen];

    // Edges
    Start -> ReadCreds;
    ReadCreds -> UserLoop;
    
    UserLoop -> ReadProfile [label="Yes (Next User)"];
    UserLoop -> End [label="Done"];
    
    ReadProfile -> ProfileExists;
    ProfileExists -> CheckUsertype [label="Yes"];
    ProfileExists -> ErrorExit_2 [label="No"];
    
    CheckUsertype -> GenStructure [label="Yes"];
    CheckUsertype -> ErrorExit [label="No"];
    
    GenStructure -> UserLoop;
}
\enddot


\section config Config files / KV files

One major structure we use is the `key=value`/`key: value` format, which we store in the **Config** data structure. \n (or user_login_t if you need hashtable. We use Config here because its gonna be o(n) anyway)

//explain parsing config here

\ref cfg_editor is used for editing profiles, and managing supplies for Nurses.