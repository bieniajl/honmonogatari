# ほんものがたり
Story Planer Tool, to help with the workflow of the story creation process.

This Project is still a work in progress, so there may be bugs.

---

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED",  "MAY", and "OPTIONAL" in this document are to be interpreted as described in [RFC 2119](https://www.rfc-editor.org/rfc/rfc2119.html).

---

## Quick Start

```bash
make config
make
bin/code
```
Keep in mind that the executable name is configuration dependent.
You can find all generated executables inside the `bin` directory.
`code` is the default debug executable and `honmonogatari` is the default release executable.



## Make Targets

|    Target    | Description                                                                                 |
|:------------:|:------------------------------------------------------------------------------------------- |
|     all      | Build all executables and documentation                                                     |
| \<default\>  | Build all executables                                                                       |
|    config    | Select the build configuration                                                              |
| printversion | Print the current program version string                                                    |
|     doc      | Build the documentation                                                                     |
|    clean     | Delete logfiles, executables and incremental build files                                    |
|    reset     | Run the clean command and delete the build configuration and compiled documentation as well |



## Versioning

This program uses semantic versioning.
See [semver.org](https://semver.org/spec/v1.0.0.html) for more details.

---

[![forthebadge](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)](https://forthebadge.com)
[![forthebadge](https://forthebadge.com/images/badges/powered-by-electricity.svg)](https://forthebadge.com)
[![forthebadge](https://forthebadge.com/images/badges/built-by-developers.svg)](https://forthebadge.com)
[![forthebadge](https://forthebadge.com/images/badges/for-you.svg)](https://forthebadge.com)
