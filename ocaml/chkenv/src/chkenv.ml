(*  chkenv - check environment

    Copyright (c) 2014-2015 Marc Butler <mockbutler@gmail.com>
    All Rights Reserved.

    To build:
    ocamlfind ocamlc -linkpkg -thread -package unix,str chkenv.ml -o chkenv.byte
    or:
    corebuild -pkg str chkenv.byte
*)

open Filename;;
open Printf;;
open Unix;;

(*  Canonize a file system path.
*)
let canonical_path path =
  if Filename.is_relative path then
    Filename.concat Filename.current_dir_name path
  else path;;

(*  Resolve a link to its unambiguous target.
    NOTE: The expectation is that link_path will be a canonical path
        otherwise the target may well be ambiguous. It is naive to think
        the current directory will always be suitable for resolving a
        relative path.
*)
let resolve_link link_path =
  let target = readlink link_path in
  if Filename.is_relative target then
    let parent = Filename.dirname link_path in
    Filename.concat parent target
  else target;;

(*  Follow one or more links to return the stat info of the final target
    file entry.
    Will raise an error if circular symbolic references are detected.
*)
let follow_links path =
  let rec follow paths_seen path =
    let full_path = canonical_path path in
    let stinfo = Unix.lstat full_path in
    if stinfo.st_kind = Unix.S_LNK then
      if List.exists (fun s -> s = full_path) paths_seen then
        failwith ("circular symlink: " ^ (List.hd (List.rev paths_seen)))
      else
        follow (full_path :: paths_seen) (resolve_link path)
    else
      stinfo in
  follow [] path;;

(*  Predicate returns true if the path is a directory.
    Follows chains of symbolic links.
*)
let is_dir path =
  let stinfo = follow_links path in
  stinfo.st_kind = S_DIR;;

(* Check and report that the path is a directory. *)
let dircheck path =
  try
    let stinfo = follow_links path in
    match stinfo.st_kind with
      S_REG -> printf "%s is a file not a directory\n" path
    | S_DIR -> ()
    | _ -> printf "%s irregular file type\n" path
  with
    Unix_error(_, fn, _) -> printf "%s %s is missing\n" fn path
  | Failure(msg) -> printf "%s\n" msg;;

(* Straight from Unix System Programming in Ocaml 2.4 *)
let iter_dir f dirname =
  let d = Unix.opendir dirname in
  try while true do f (readdir d) done
  with End_of_file -> closedir d;;

(* Return the PATH environment as a list of paths. *)
let get_path =
  Str.split (Str.regexp ":") (Unix.getenv "PATH");;

(* Entry point. *)
let _ =
  let dirs = get_path in
  List.iter dircheck dirs ;;
