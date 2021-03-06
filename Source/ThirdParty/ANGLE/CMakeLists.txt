include_directories(
    src
    include
    include/KHR
)

if (${CMAKE_GENERATOR} MATCHES "Ninja")
    if (${MSVC_CXX_ARCHITECTURE_ID} STREQUAL "X86")
        link_directories(${WINDOWSSDKDIR}Lib/${WINDOWSSDKLIBVERSION}/um/x86)
    else ()
        link_directories(${WINDOWSSDKDIR}Lib/${WINDOWSSDKLIBVERSION}/um/x64)
    endif ()
else ()
    if (${MSVC_CXX_ARCHITECTURE_ID} STREQUAL "X86")
        link_directories($(WINDOWSSDKDIR)Lib/$(WINDOWSSDKLIBVERSION)/um/x86)
    else ()
        link_directories($(WINDOWSSDKDIR)Lib/$(WINDOWSSDKLIBVERSION)/um/x64)
    endif ()
endif ()

add_library(libEGL SHARED
    src/libEGL/libEGL.cpp
    src/libEGL/libEGL.def
    src/libEGL/libEGL.rc
)

set_property(TARGET libEGL PROPERTY COMPILE_DEFINITIONS
    ANGLE_WEBKIT_WIN
    EGLAPI=
)

add_library(libGLESv2 SHARED
    src/libGLESv2/entry_points_egl.cpp
    src/libGLESv2/entry_points_egl_ext.cpp
    src/libGLESv2/entry_points_gles_2_0.cpp
    src/libGLESv2/entry_points_gles_2_0_ext.cpp
    src/libGLESv2/entry_points_gles_3_0.cpp
    src/libGLESv2/entry_points_gles_3_0_ext.cpp
    src/libGLESv2/global_state.cpp
    src/libGLESv2/libGLESv2.cpp
    src/libGLESv2/libGLESv2.def
    src/libGLESv2/libGLESv2.rc
)

set_property(TARGET libGLESv2 PROPERTY COMPILE_DEFINITIONS
    ANGLE_WEBKIT_WIN
    GL_APICALL=
    LIBGLESV2_IMPLEMENTATION
)

add_library(libANGLE STATIC
    src/common/angleutils.cpp
    src/common/debug.cpp
    src/common/event_tracer.cpp
    src/common/mathutil.cpp
    src/common/MemoryBuffer.cpp
    src/common/tls.cpp
    src/common/utilities.cpp
    src/compiler/preprocessor/DiagnosticsBase.cpp
    src/compiler/preprocessor/DirectiveHandlerBase.cpp
    src/compiler/preprocessor/DirectiveParser.cpp
    src/compiler/preprocessor/ExpressionParser.cpp
    src/compiler/preprocessor/Input.cpp
    src/compiler/preprocessor/Lexer.cpp
    src/compiler/preprocessor/Macro.cpp
    src/compiler/preprocessor/MacroExpander.cpp
    src/compiler/preprocessor/Preprocessor.cpp
    src/compiler/preprocessor/Token.cpp
    src/compiler/preprocessor/Tokenizer.cpp
    src/compiler/translator/BuiltInFunctionEmulatorGLSL.cpp
    src/compiler/translator/OutputESSL.cpp
    src/compiler/translator/OutputGLSL.cpp
    src/compiler/translator/OutputGLSLBase.cpp
    src/compiler/translator/SearchSymbol.cpp
    src/compiler/translator/TranslatorESSL.cpp
    src/compiler/translator/TranslatorGLSL.cpp
    src/compiler/translator/UnfoldShortCircuit.cpp
    src/compiler/translator/VersionGLSL.cpp
    src/compiler/translator/ASTMetadataHLSL.cpp
    src/compiler/translator/blocklayoutHLSL.cpp
    src/compiler/translator/BuiltInFunctionEmulatorHLSL.cpp
    src/compiler/translator/OutputHLSL.cpp
    src/compiler/translator/SearchSymbol.cpp
    src/compiler/translator/StructureHLSL.cpp
    src/compiler/translator/TranslatorHLSL.cpp
    src/compiler/translator/UnfoldShortCircuit.cpp
    src/compiler/translator/UniformHLSL.cpp
    src/compiler/translator/UtilsHLSL.cpp
    src/compiler/translator/ArrayReturnValueToOutParameter.cpp
    src/compiler/translator/blocklayout.cpp
    src/compiler/translator/BuiltInFunctionEmulator.cpp
    src/compiler/translator/CallDAG.cpp
    src/compiler/translator/CodeGen.cpp
    src/compiler/translator/Compiler.cpp
    src/compiler/translator/Diagnostics.cpp
    src/compiler/translator/DirectiveHandler.cpp
    src/compiler/translator/EmulatePrecision.cpp
    src/compiler/translator/FlagStd140Structs.cpp
    src/compiler/translator/ForLoopUnroll.cpp
    src/compiler/translator/InfoSink.cpp
    src/compiler/translator/Initialize.cpp
    src/compiler/translator/InitializeVariables.cpp
    src/compiler/translator/InitializeDll.cpp
    src/compiler/translator/InitializeParseContext.cpp
    src/compiler/translator/Intermediate.cpp
    src/compiler/translator/IntermNode.cpp
    src/compiler/translator/intermOut.cpp
    src/compiler/translator/IntermTraverse.cpp
    src/compiler/translator/LoopInfo.cpp
    src/compiler/translator/Operator.cpp
    src/compiler/translator/parseConst.cpp
    src/compiler/translator/ParseContext.cpp
    src/compiler/translator/PoolAlloc.cpp
    src/compiler/translator/PruneEmptyDeclarations.cpp
    src/compiler/translator/QualifierAlive.cpp
    src/compiler/translator/RegenerateStructNames.cpp
    src/compiler/translator/RemoveSwitchFallThrough.cpp
    src/compiler/translator/RewriteElseBlocks.cpp
    src/compiler/translator/ScalarizeVecAndMatConstructorArgs.cpp
    src/compiler/translator/SearchSymbol.cpp
    src/compiler/translator/SeparateArrayInitialization.cpp
    src/compiler/translator/SeparateDeclarations.cpp
    src/compiler/translator/ShaderLang.cpp
    src/compiler/translator/ShaderVars.cpp
    src/compiler/translator/SimplifyArrayAssignment.cpp
    src/compiler/translator/SymbolTable.cpp
    src/compiler/translator/Types.cpp
    src/compiler/translator/UnfoldShortCircuit.cpp
    src/compiler/translator/UnfoldShortCircuitAST.cpp
    src/compiler/translator/util.cpp
    src/compiler/translator/ValidateLimitations.cpp
    src/compiler/translator/ValidateOutputs.cpp
    src/compiler/translator/ValidateSwitch.cpp
    src/compiler/translator/VariableInfo.cpp
    src/compiler/translator/VariablePacker.cpp
    src/compiler/translator/glslang_lex.cpp
    src/compiler/translator/glslang_tab.cpp
    src/compiler/translator/depgraph/DependencyGraph.cpp
    src/compiler/translator/depgraph/DependencyGraphBuilder.cpp
    src/compiler/translator/depgraph/DependencyGraphOutput.cpp
    src/compiler/translator/depgraph/DependencyGraphTraverse.cpp
    src/compiler/translator/timing/RestrictFragmentShaderTiming.cpp
    src/compiler/translator/timing/RestrictVertexShaderTiming.cpp
    src/compiler/translator/VersionGLSL.cpp
    src/libANGLE/angletypes.cpp
    src/libANGLE/AttributeMap.cpp
    src/libANGLE/Buffer.cpp
    src/libANGLE/Caps.cpp
    src/libANGLE/Compiler.cpp
    src/libANGLE/Config.cpp
    src/libANGLE/Context.cpp
    src/libANGLE/Data.cpp
    src/libANGLE/Device.cpp
    src/libANGLE/Display.cpp
    src/libANGLE/Error.cpp
    src/libANGLE/Fence.cpp
    src/libANGLE/Float16ToFloat32.cpp
    src/libANGLE/formatutils.cpp
    src/libANGLE/Framebuffer.cpp
    src/libANGLE/FramebufferAttachment.cpp
    src/libANGLE/HandleAllocator.cpp
    src/libANGLE/ImageIndex.cpp
    src/libANGLE/Platform.cpp
    src/libANGLE/Program.cpp
    src/libANGLE/Query.cpp
    src/libANGLE/queryconversions.cpp
    src/libANGLE/RefCountObject.cpp
    src/libANGLE/Renderbuffer.cpp
    src/libANGLE/renderer/d3d/BufferD3D.cpp
    src/libANGLE/renderer/d3d/CompilerD3D.cpp
    src/libANGLE/renderer/d3d/copyimage.cpp
    src/libANGLE/renderer/d3d/d3d11/Blit11.cpp
    src/libANGLE/renderer/d3d/d3d11/Buffer11.cpp
    src/libANGLE/renderer/d3d/d3d11/Clear11.cpp
    src/libANGLE/renderer/d3d/d3d11/DebugAnnotator11.cpp
    src/libANGLE/renderer/d3d/d3d11/Fence11.cpp
    src/libANGLE/renderer/d3d/d3d11/formatutils11.cpp
    src/libANGLE/renderer/d3d/d3d11/Framebuffer11.cpp
    src/libANGLE/renderer/d3d/d3d11/Image11.cpp
    src/libANGLE/renderer/d3d/d3d11/IndexBuffer11.cpp
    src/libANGLE/renderer/d3d/d3d11/InputLayoutCache.cpp
    src/libANGLE/renderer/d3d/d3d11/PixelTransfer11.cpp
    src/libANGLE/renderer/d3d/d3d11/Query11.cpp
    src/libANGLE/renderer/d3d/d3d11/Renderer11.cpp
    src/libANGLE/renderer/d3d/d3d11/renderer11_utils.cpp
    src/libANGLE/renderer/d3d/d3d11/RenderStateCache.cpp
    src/libANGLE/renderer/d3d/d3d11/RenderTarget11.cpp
    src/libANGLE/renderer/d3d/d3d11/ShaderExecutable11.cpp
    src/libANGLE/renderer/d3d/d3d11/SwapChain11.cpp
    src/libANGLE/renderer/d3d/d3d11/TextureStorage11.cpp
    src/libANGLE/renderer/d3d/d3d11/Trim11.cpp
    src/libANGLE/renderer/d3d/d3d11/VertexBuffer11.cpp
    src/libANGLE/renderer/d3d/d3d11/win32/NativeWindow.cpp
    src/libANGLE/renderer/d3d/d3d9/Blit9.cpp
    src/libANGLE/renderer/d3d/d3d9/Buffer9.cpp
    src/libANGLE/renderer/d3d/d3d9/DebugAnnotator9.cpp
    src/libANGLE/renderer/d3d/d3d9/Fence9.cpp
    src/libANGLE/renderer/d3d/d3d9/formatutils9.cpp
    src/libANGLE/renderer/d3d/d3d9/Framebuffer9.cpp
    src/libANGLE/renderer/d3d/d3d9/Image9.cpp
    src/libANGLE/renderer/d3d/d3d9/IndexBuffer9.cpp
    src/libANGLE/renderer/d3d/d3d9/Query9.cpp
    src/libANGLE/renderer/d3d/d3d9/Renderer9.cpp
    src/libANGLE/renderer/d3d/d3d9/renderer9_utils.cpp
    src/libANGLE/renderer/d3d/d3d9/RenderTarget9.cpp
    src/libANGLE/renderer/d3d/d3d9/ShaderExecutable9.cpp
    src/libANGLE/renderer/d3d/d3d9/SwapChain9.cpp
    src/libANGLE/renderer/d3d/d3d9/TextureStorage9.cpp
    src/libANGLE/renderer/d3d/d3d9/VertexBuffer9.cpp
    src/libANGLE/renderer/d3d/d3d9/VertexDeclarationCache.cpp
    src/libANGLE/renderer/d3d/DeviceD3D.cpp
    src/libANGLE/renderer/d3d/DisplayD3D.cpp
    src/libANGLE/renderer/d3d/DynamicHLSL.cpp
    src/libANGLE/renderer/d3d/formatutilsD3D.cpp
    src/libANGLE/renderer/d3d/FramebufferD3D.cpp
    src/libANGLE/renderer/d3d/HLSLCompiler.cpp
    src/libANGLE/renderer/d3d/ImageD3D.cpp
    src/libANGLE/renderer/d3d/IndexBuffer.cpp
    src/libANGLE/renderer/d3d/IndexDataManager.cpp
    src/libANGLE/renderer/d3d/loadimage.cpp
    src/libANGLE/renderer/d3d/loadimageSSE2.cpp
    src/libANGLE/renderer/d3d/ProgramD3D.cpp
    src/libANGLE/renderer/d3d/RenderbufferD3D.cpp
    src/libANGLE/renderer/d3d/RendererD3D.cpp
    src/libANGLE/renderer/d3d/RenderTargetD3D.cpp
    src/libANGLE/renderer/d3d/ShaderD3D.cpp
    src/libANGLE/renderer/d3d/ShaderExecutableD3D.cpp
    src/libANGLE/renderer/d3d/SurfaceD3D.cpp
    src/libANGLE/renderer/d3d/TextureD3D.cpp
    src/libANGLE/renderer/d3d/TextureStorage.cpp
    src/libANGLE/renderer/d3d/TransformFeedbackD3D.cpp
    src/libANGLE/renderer/d3d/VertexBuffer.cpp
    src/libANGLE/renderer/d3d/VertexDataManager.cpp
    src/libANGLE/renderer/DeviceImpl.cpp
    src/libANGLE/renderer/DisplayImpl.cpp
    src/libANGLE/renderer/IndexRangeCache.cpp
    src/libANGLE/renderer/ProgramImpl.cpp
    src/libANGLE/renderer/RenderbufferImpl.cpp
    src/libANGLE/renderer/Renderer.cpp
    src/libANGLE/renderer/SurfaceImpl.cpp
    src/libANGLE/ResourceManager.cpp
    src/libANGLE/Sampler.cpp
    src/libANGLE/Shader.cpp
    src/libANGLE/State.cpp
    src/libANGLE/Surface.cpp
    src/libANGLE/Texture.cpp
    src/libANGLE/TransformFeedback.cpp
    src/libANGLE/Uniform.cpp
    src/libANGLE/validationEGL.cpp
    src/libANGLE/validationES.cpp
    src/libANGLE/validationES2.cpp
    src/libANGLE/validationES3.cpp
    src/libANGLE/VertexArray.cpp
    src/libANGLE/VertexAttribute.cpp
    src/third_party/compiler/ArrayBoundsClamper.cpp
    src/third_party/murmurhash/MurmurHash3.cpp
    src/third_party/systeminfo/SystemInfo.cpp
)

set_property(TARGET libANGLE PROPERTY COMPILE_DEFINITIONS
    ANGLE_DEFAULT_D3D11=1
    ANGLE_ENABLE_D3D9
    ANGLE_ENABLE_D3D11
    ANGLE_ENABLE_HLSL
    ANGLE_SKIP_DXGI_1_2_CHECK=1
    ANGLE_WEBKIT_WIN
    LIBANGLE_IMPLEMENTATION
    NOMINMAX
    __STDC_LIMIT_MACROS
)

target_link_libraries(libEGL
    libGLESv2
)

target_link_libraries(libGLESv2
    D3d9
    libANGLE
)

set_target_properties(libANGLE PROPERTIES FOLDER "ANGLE")
set_target_properties(libEGL PROPERTIES FOLDER "ANGLE")
set_target_properties(libGLESv2 PROPERTIES FOLDER "ANGLE")
