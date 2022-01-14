:: compile any .vert or .frag files with glslc

for /r %~dp0 %%f in (*.*) do (
	 if "%%~xf" == ".vert" glslc.exe %%f -o %~dp0%%~nf.spv
 	 if "%%~xf" == ".frag" glslc.exe %%f -o %~dp0%%~nf.spv
)
