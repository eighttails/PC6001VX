# AGENTS.md

Guidance for AI coding agents working in this repository.

## Project Overview

PC6001VX is a C++17 Qt 6 application. The primary desktop UI code lives under `src/Qt/`, while emulator core code lives under `src/`. The project is built with CMake.

## General Working Rules

- Make focused, minimal changes that directly address the requested task.
- Preserve existing behavior unless the user explicitly asks for a behavior change.
- Follow the surrounding code style and naming conventions.
- Do not reformat unrelated code.
- Do not remove or overwrite user changes.
- Avoid committing secrets, credentials, local paths, or machine-specific configuration.
- Update documentation when it is directly affected by the change.

## Language and Responses

- Respond to the user in the same language used by the user.
- If the user writes in Japanese, respond in Japanese.
- Keep generated project documentation in the language requested by the user.

## Build and Verification

- Use Ninja for build verification.
- Prefer an out-of-tree build directory.
- A typical verification flow is:

```sh
cmake -S . -B /tmp/pc6001vx-ninja-check -G Ninja
ninja -C /tmp/pc6001vx-ninja-check PC6001VX
```

- If optional dependencies are unavailable and the task does not concern them, it is acceptable to configure with project options such as:

```sh
cmake -S . -B /tmp/pc6001vx-ninja-check -G Ninja -DNOAVI=ON -DNOSOUND=ON
ninja -C /tmp/pc6001vx-ninja-check PC6001VX
```

- Clean temporary build directories after verification when they are no longer needed.

## CMake and Dependencies

- Do not introduce new build tools unless necessary.
- Keep CMake changes scoped to the files and targets affected by the task.
- When adding Qt sources, headers, QML files, or resources, ensure they are registered in the appropriate CMake source lists and `.qrc` files.

## Qt/QML Guidelines

- Keep UI-thread-only Qt object manipulation on the main thread.
- Preserve existing signal/slot behavior and public APIs where possible.
- When replacing widgets or graphics code, maintain compatibility with existing callers unless the task explicitly asks for an API change.
- For QML resources, ensure the paths are available through the Qt resource system.

## Testing and Validation

- Run the smallest relevant build or test that validates the change.
- For documentation-only changes, a build is not required unless documentation generation or validation is part of the project.
- If validation cannot be completed, clearly state what was attempted and why it could not be completed.

## Git Practices

- Check the working tree before making broad changes.
- Do not use destructive Git commands unless explicitly requested.
- Do not amend or create commits unless the user asks for a commit.
