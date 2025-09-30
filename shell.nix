{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShellNoCC {
  buildInputs = with pkgs; [
    gcc15
    libxcrypt
    curl
    gdb
    
    # C man pages
    glibcInfo
    man-pages

    # make
    gnumake

    sqlite
 ];
}
