{ pkgs ? (import <nixpkgs> {}) }:

pkgs.mkShell {
  nativeBuildInputs = with pkgs.buildPackages; [
    gcc
    bear
    ccls
    gnumake
    sfml
    gdb
  ];
}
