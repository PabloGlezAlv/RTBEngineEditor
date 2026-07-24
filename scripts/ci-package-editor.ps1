# Build RTBEngineEditor + RTBPlayer + GameScripts and package a runnable folder.
param(
    [string]$WorkspaceRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path,
    [string]$AssimpToolset = '',
    [string]$PlatformToolset = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$engineRoot = Join-Path $WorkspaceRoot 'RTBEngine'
$editorRoot = Join-Path $WorkspaceRoot 'RTBEngineEditor'
$sdkDir = Join-Path $editorRoot 'RTBEngineEditor\RTBEngine_SDK'

. (Join-Path $engineRoot 'scripts\ci-common.ps1')

$toolchain = Get-RtbToolchain -PreferredAssimpToolset $AssimpToolset -PreferredPlatformToolset $PlatformToolset
Write-Host "Toolchain: PlatformToolset=$($toolchain.PlatformToolset) Assimp=$($toolchain.AssimpToolset)"

& (Join-Path $engineRoot 'scripts\ci-build-sdk.ps1') `
    -EngineRoot $engineRoot `
    -OutputDir $sdkDir `
    -AssimpToolset $toolchain.AssimpToolset `
    -PlatformToolset $toolchain.PlatformToolset

$msbuildProps = @{
    PlatformToolset  = $toolchain.PlatformToolset
    RTBAssimpToolset = $toolchain.AssimpToolset
}

Invoke-MsBuild -SolutionOrProject (Join-Path $editorRoot 'RTBEngineEditor\RTBEngineEditor.sln') -Configuration Release -Properties $msbuildProps
Invoke-MsBuild -SolutionOrProject (Join-Path $editorRoot 'RTBPlayer\RTBPlayer.sln') -Configuration Release -Properties $msbuildProps
Invoke-MsBuild -SolutionOrProject (Join-Path $editorRoot 'RTBEngineEditor\GameScripts\GameScripts.vcxproj') -Configuration Release -Properties $msbuildProps

$outDir = Join-Path $editorRoot 'RTBEngineEditor\x64\Release'
if (-not (Test-Path (Join-Path $outDir 'RTBEngineEditor.exe'))) {
    throw "Editor output not found at $outDir"
}

$packageRoot = Join-Path $editorRoot 'dist\RTBEngineEditor'
if (Test-Path $packageRoot) {
    Remove-Item -Recurse -Force $packageRoot
}
New-Item -ItemType Directory -Force -Path $packageRoot | Out-Null

Write-Host "Packaging editor from $outDir"
Copy-Item -Recurse -Force (Join-Path $outDir '*') $packageRoot

# Ensure player and scripts are present at package root.
$playerExe = Join-Path $sdkDir 'Bin\RTBPlayer.exe'
if (Test-Path $playerExe) {
    Copy-Item -Force $playerExe (Join-Path $packageRoot 'RTBPlayer.exe')
    Copy-Item -Force $playerExe (Join-Path $packageRoot 'RTBEngine_SDK\Bin\RTBPlayer.exe')
}

$gameScriptsDll = Join-Path $editorRoot 'RTBEngineEditor\x64\Release\GameScripts.dll'
if (Test-Path $gameScriptsDll) {
    Copy-Item -Force $gameScriptsDll (Join-Path $packageRoot 'GameScripts.dll')
}

Write-Host "Editor package ready at: $packageRoot"
