(*
Dump the header of a Password Safe Version 3 file.

Notes

- Cannot build with Ocaml 4.08 as it gets errors about Stdlib.Pervasives. I
  wonder if this has something to do with the ppx deriver.
*)

open Core


let dump_field name field =
  printf "%s\n"  name;
  Bitstring.hexdump_bitstring stdout field


let psafe3_dump file () =
  let bits = Bitstring.bitstring_of_file file in
  match%bitstring bits with
  | {| "PWS3" : 4 * 8 : string;
      salt : 256 : bitstring;
      iter : 32 : littleendian;
      stretched_key : 256 : bitstring;
      stretched_key_hash : 256 : bitstring;
      b1 : 128 : bitstring;
      b2 : 128 : bitstring;
      b3 : 128 : bitstring;
      b4 : 128 : bitstring;
      iv : 128 : bitstring;
      _ : -1 : bitstring
    |} ->
    printf "PSAFE3 FILE\n";
    printf "Iterations = %ld\n" iter;
    dump_field "Salt" salt;
    dump_field "P'" stretched_key;
    dump_field "P' Hash" stretched_key_hash;
    dump_field "B1" b1;
    dump_field "B2" b2;
    dump_field "B3" b3;
    dump_field "B4" b4;
    dump_field "IV" iv;
  | {| _ |} ->
    failwith "Not a psafe3 file."


let filename_param = Command.Param.(anon ("filename" %: string))


let command =
  Command.basic
    ~summary:"Dump psafe3 file prologue."
    ~readme:(fun () -> "Does not decrypt the file.")
    (Command.Param.map filename_param  ~f:(fun filename -> psafe3_dump filename))


let () =
  Command.run ~version:"0.1" ~build_info:"mockbutler@gmail.com" command
