# Duktape JSI

[JSI](https://github.com/facebook/hermes/tree/main/API/jsi/jsi)
bindings for the [Duktape JavaScript](https://duktape.org) runtime.

I started this as an experimental project, but it seems fairly
complete now. All JSI tests are passing (with [one minor
exception](https://github.com/svaarala/duktape/issues/2491)). In
theory, this could be used in the following ways:

1. a runtime-agnostic C++ wrapper around Duktape
2. an alternative JavaScript engine for [React Native](https://github.com/facebook/react-native)

I haven't used this in any projects (yet) so no guarantees. But feel
free to file an issue if you'd like me to help you work through a use
case. I'd love to figure out how to compile this as part of React
Native.
