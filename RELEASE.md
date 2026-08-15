# Releasing Construo

Version numbers come only from the top-level **`VERSION`** file. See `AGENTS.md`.

## Development (in git)

`VERSION` contains a `-dev` suffix, e.g.:

```text
0.2.3-dev
```

Builds expand this to:

```text
0.2.3-dev.<revCount>+g<shortHash>[-dirty]
```

via CMake (`GetProjectVersion.cmake`) and the Nix flake (`-DPROJECT_VERSION_FULL=…`).

Check with:

```bash
./scripts/print-version.sh
nix build .#construo-sdl -L
result/bin/construo --version
```

## Cutting a release

1. Ensure `nix flake check` is green.
2. Set `VERSION` to the release number **without** `-dev` (e.g. `0.2.3`).
3. Update `NEWS` with user-visible changes.
4. Commit: `Release version 0.2.3`
5. Tag: `git tag -a v0.2.3 -m "Construo 0.2.3"` (tag = `v` + `VERSION`).
6. Push commit and tag.
7. Immediately after, set `VERSION` back to the next development version
   (e.g. `0.2.4-dev`) and commit.

Release tarballs / Nix builds must **not** append git metadata when `VERSION`
has no `-dev` suffix.
