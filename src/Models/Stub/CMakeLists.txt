# Build the cyclus executable from the CYCLUS_SRC source files
ADD_SUBDIRECTORY( StubStub )

SET(ModelTestSource ${ModelTestSource} ${StubTestSource} PARENT_SCOPE)
