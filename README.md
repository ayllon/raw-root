#raw-root

[RAW](https://github.com/MiguelBranco/raw "RAW") backend for ROOT files.

## Build
raw-root uses [sbt](http://www.scala-sbt.org/ "sbt") to orchestrate the build, with the plugin [sbt-cpp](https://github.com/d40cht/sbt-cpp "sbt-cpp") enabled to build the native bits.

You can build raw-root in two different manners:

Directly using sbt:
```bash
sbt "nativeBuildConfiguration <sbt configuration>" compile
```

Using the convenience Makefile:
```bash
make [stage|clean]
```

The target "stage" will create a directory "stage" with all the artifacts that have been built.


Since the Makefile is there just for convenience, in both cases sbt will retrieve and build by itself sbt-cpp and raw as dependencies. Additionally, you will need to have the development headers of [ROOT](http://root.cern.ch/drupal/ "ROOT") and JNI installed.

Their paths are currenly hardcoded in [project/Build.scala](https://github.com/ayllon/raw-root/blob/master/project/Build.scala "Build.scala").
