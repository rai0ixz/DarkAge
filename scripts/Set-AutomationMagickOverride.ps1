# Set-AutomationMagickOverride.ps1
$ErrorActionPreference = 'Stop'

$Target = 'D:\UE\UE_5.6\Engine\Source\Programs\AutomationTool\Directory.Build.props'
$Dir = Split-Path -LiteralPath $Target

if (-not (Test-Path -LiteralPath $Dir)) {
    New-Item -ItemType Directory -Force -Path $Dir | Out-Null
}

# Central props to:
# - Override Magick.NET to a safe version
# - Ensure NuGet vulnerability warnings (NU1901-NU1904) are NOT treated as errors during restore
$xml = @"
<Project>
  <PropertyGroup>
    <RestoreNoWarn>$(RestoreNoWarn);NU1901;NU1902;NU1903;NU1904</RestoreNoWarn>
    <RestoreTreatWarningsAsErrors>false</RestoreTreatWarningsAsErrors>
    <RestoreWarningsAsErrors></RestoreWarningsAsErrors>
    <NuGetAuditMode>direct</NuGetAuditMode>
  </PropertyGroup>
  <ItemGroup>
    <PackageReference Update="Magick.NET-Q16-HDRI-AnyCPU" Version="14.11.0" />
  </ItemGroup>
</Project>
"@

Set-Content -LiteralPath $Target -Value $xml -Encoding UTF8

Write-Host "WROTE: $Target"