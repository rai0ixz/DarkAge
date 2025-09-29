# Set-AutomationRestoreTargets.ps1
$ErrorActionPreference = 'Stop'

$Target = 'D:\UE\UE_5.6\Engine\Source\Programs\AutomationTool\Directory.Build.targets'
$Dir = Split-Path -LiteralPath $Target

if (-not (Test-Path -LiteralPath $Dir)) {
    New-Item -ItemType Directory -Force -Path $Dir | Out-Null
}

# Ensure restore does not treat NU1901-NU1904 as errors and suppress them
$content = @"
<Project>
  <PropertyGroup>
    <RestoreTreatWarningsAsErrors>false</RestoreTreatWarningsAsErrors>
    <TreatWarningsAsErrors>false</TreatWarningsAsErrors>
    <RestoreNoWarn>$(RestoreNoWarn);NU1901;NU1902;NU1903;NU1904</RestoreNoWarn>
    <NoWarn>$(NoWarn);NU1901;NU1902;NU1903;NU1904</NoWarn>
  </PropertyGroup>
</Project>
"@

Set-Content -LiteralPath $Target -Value $content -Encoding UTF8
Write-Host "WROTE: $Target"