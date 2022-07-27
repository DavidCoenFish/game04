#PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& (Get-Item '%8').LastWriteTime = (Get-Item '%~9').LastWriteTime"

$destination=$args[0]
$source=$args[1]

(Get-Item $destination).LastWriteTime = (Get-Item $source).LastWriteTime