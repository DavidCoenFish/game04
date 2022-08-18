#Get-FileHash -Algorithm MD5 -Path (Get-ChildItem "C:\FooFolder\baseball2\*.*" -Recurse -force)

$versionFilePath=$args[0]
$sciptDir=$args[1]

$foo = Get-Location
Write-Host($foo)
Write-Host($versionFilePath)
Write-Host($sciptDir)

