\page db The user database

We make use of files and folders to create our "database" (**the users/ folder**, if you haven't renamed it), which can be described in file_struct.yaml.txt:

YAML format:
@code{.yaml}
Usertype:
  - path: "users/%s/" # This is a format string, %s is for the username
    type: directory
  
  - path: "users/%s/prefs.txt"
    type: file
    content:
      key: value
      blabla: blablabla

Usertype2:
  - path: "users/%s/"
  - type: directory

  - path: "users/%s/profile.txt"
...
@endcode
./users contain folders of usernames, where each stores their own data



This file is parsed in regen_file_structure.c (which is meant to generate one exe).
\n it regenerates the file structure while ignoring existing files and lines to prevent data loss.

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

TODO: Make flowchart for generate_file_structure()


\section config Config files

One major structure we use is the `key=value`/`key: value` format, which we store in the **Config** data structure. \n (or user_login_t if you need hashtable. We use Config here because its gonna be o(n) anyway)

\n The util functions for **Config** are:
