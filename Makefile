SBT=$(shell which sbt)
SBT_CPP_CONFIG=Gcc_LinuxPC_Debug
SBT_CPP_COFIG_PATH=$(subst _,/,$(SBT_CPP_CONFIG))

all:
	$(SBT) "nativeBuildConfiguration $(SBT_CPP_CONFIG)" compile package

stage: all
	mkdir -p stage
	mkdir -p stage/walker stage/walker/handlers
	install target/native/RawRootPlugin/RootWalker/$(SBT_CPP_COFIG_PATH)/RootWalker.so stage/walker
	install target/native/RawRootPlugin/Handler_*/$(SBT_CPP_COFIG_PATH)/*.so stage/walker/handlers
	install target/native/RawRootPlugin/RawRootPluginNative/$(SBT_CPP_COFIG_PATH)/RawRootPluginNative.so stage
	install RawRootPlugin/target/scala-*/rawrootpluginjvm*.jar stage

clean:
	rm -rf stage
	rm -rf target
	rm -rf project/target
	rm -rf project/project/target
	rm -rf RawRootPlugin/target
