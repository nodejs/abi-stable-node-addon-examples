{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "addon.cc" ],
      "include_dirs": [
        "<!(node -p \"require('node-api').include\")"
      ]
    }
  ]
}
