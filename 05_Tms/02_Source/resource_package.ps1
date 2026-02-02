# Check if argument is provided
if ($args.Count -ne 1) {
    Write-Host "Usage: md6.ps1 <zip_file>"
    exit 1
}

# Get the zip file parameter
$zipFile = $args[0]

# Check if file exists
if (!(Test-Path $zipFile)) {
    Write-Host "File not found!"
    exit 1
}

# Calculate MD5 hash of the zip file
$md5 = (Get-FileHash $zipFile -Algorithm MD5).Hash.ToLower()

# Get the filename
$filename = Split-Path $zipFile -Leaf

Write-Host "Filename: $filename"
Write-Host "MD5 Value: $md5"

# Extract version from filename (e.g. V1.0.6)
$versionMatch = [regex]::Match($filename, 'V\d+\.\d+(\.\d+)?')
$version = if ($versionMatch.Success) { $versionMatch.Value } else { "" }

Write-Host "Version: $version"

# Read the original resource.json content to preserve formatting including line endings and encoding
$originalContent = [System.IO.File]::ReadAllText("resource.json", [System.Text.Encoding]::UTF8)

# Update the JSON properties using regex to preserve original formatting
$updatedContent = $originalContent -replace '(?m)^(\s*"fileName":\s*")[^"]*(".*)$', "`${1}$filename`${2}"
$updatedContent = $updatedContent -replace '(?m)^(\s*"md5":\s*")[^"]*(".*)$', "`${1}$md5`${2}"
if ($version) {
    $updatedContent = $updatedContent -replace '(?m)^(\s*"version":\s*")[^"]*(".*)$', "`${1}$version`${2}"
}

# Write the updated content back to the file, preserving original UTF-8 encoding without BOM
$utf8WithoutBom = New-Object System.Text.UTF8Encoding($false)
[System.IO.File]::WriteAllText("resource.json", $updatedContent, $utf8WithoutBom)

# Read the updated JSON to extract values for the zip naming
$json = ConvertFrom-Json $updatedContent
$packageType = $json.packageType
$machineType = $json.machineType
$customerName = $json.customerName

# Set MQ as fixed value since moduleType field is not in JSON anymore
$mqType = "MQ"

# Build new zip file name
$newZipName = "${packageType}_${machineType}-${mqType}_${customerName}_${version}.zip"
Write-Host "New package name: $newZipName"

# Create new zip file containing resource.json and the source zip file
Compress-Archive -Path @("resource.json", $zipFile) -DestinationPath $newZipName -Force

Write-Host "MD5 hash, version, and file information updated successfully in resource.json"
Write-Host "New package created: $newZipName"