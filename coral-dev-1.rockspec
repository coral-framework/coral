package = "coral"
version = "dev-1"
source = {
  url = "git://github.com/coral-framework/coral"
}
description = {
  summary = "Modern component-based C++ development framework.",
  detailed = [[
    Coral is a modern component-based C++ development framework.
  ]],
  homepage = "http://www.coral-framework.org",
  license = "MIT"
}
dependencies = {
  'lua ~> 5.2',
  'lpeg ~> 0.12',
  'busted ~> 2.0',
}
build = {
  type = "builtin",
  modules = {},
}
