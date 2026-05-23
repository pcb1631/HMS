\page db The user "database"

We make use of files and folders to create our database, which can be described in the file_struct.yaml.txt:

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
@endcode
./users contain folders of usernames, where each stores their own data



This file is parsed in regen_file_structure.c (which is meant to generate one exe).
\n it regenerates the file structure while ignoring existing files and lines to prevent data loss.

\dot
digraph regen_flow {
    node [shape=box, fontname="Helvetica", fontsize=10];
    edge [fontname="Helvetica", fontsize=10];
    
    start [shape=oval, label="Start"];
    read_creds [label="Parse ./users/creds.txt\n(Get list of users)"];
    loop_check [shape=diamond, label="For each\nuser?"];
    read_profile [label="Parse ./users/<username>/profile.txt\n(Get USERTYPE)"];
    gen_struct [label="generate_file_structure(USERTYPE, username)"];
    end [shape=oval, label="End"];

    start -> read_creds;
    read_creds -> loop_check;
    loop_check -> read_profile [label="Yes"];
    read_profile -> gen_struct;
    gen_struct -> loop_check;
    loop_check -> end [label="No"];
}
\enddot

One major structure we use is the key=value format, which we store in the **Config** data structure. (or user_login_t if you need hashtable)
\n The util functions for **Config** are:

- a
