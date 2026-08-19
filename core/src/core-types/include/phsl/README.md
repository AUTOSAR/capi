# phsl: Abseil C++ subset

This directory contains a vendored subset of the Abseil C++ library, based on the upstream version `20220623.2`.

## Purpose

The CAPI implementation is built with C++14. Since `std::variant` is only available starting with C++17, this project uses the `variant` implementation provided by Abseil.

During the Android port, a conflict with Android's built-in Abseil C++ copy was identified. To allow both copies to coexist, the vendored Abseil subset was namespace- and prefix-relocated.

Only the files required for this use case are included, including the required headers and `bad_variant_access.cc`.

## Modifications

The vendored code has been modified only as follows:

* `absl` was renamed to `phsl`
* `ABSL_*` macros were renamed to `PBSL_*`
* corresponding include paths were adjusted

No functional changes were made beyond this relocation.

## Upstream maintenance

When updating this code, changes should be taken from the corresponding Abseil C++ upstream version and the namespace/prefix relocation must be applied consistently.

Changes beyond the relocation require separate review and documentation.

If the project is updated to C++17 or later, it must be evaluated whether this vendored subset can be replaced with the standard library `std::variant` implementation and removed.
