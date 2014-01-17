lazy val root = project.in(file(".")).dependsOn(cppPlugin)
lazy val cppPlugin = uri("git://github.com/d40cht/sbt-cpp.git")
