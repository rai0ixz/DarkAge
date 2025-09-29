param(
    [string]$Root = 'D:\UE\UE_5.6\Engine\Source\Programs\AutomationTool',
    [string]$Version = '14.11.0',
    [switch]$WhatIf
)

$ErrorActionPreference = 'Stop'
Write-Host "Scanning $Root for Magick.NET upgrades to version $Version"

function Update-Csproj {
    param(
        [Parameter(Mandatory=$true)][string]$Path,
        [Parameter(Mandatory=$true)][string]$Version
    )
    try {
        [xml]$xml = Get-Content -Raw -LiteralPath $Path

        # Find PackageReference nodes regardless of XML namespace
        $nodes = $xml.SelectNodes("//*[local-name()='PackageReference' and (@Include='Magick.NET-Q16-HDRI-AnyCPU' or @Update='Magick.NET-Q16-HDRI-AnyCPU')]")
        if (-not $nodes -or $nodes.Count -eq 0) {
            return $false
        }

        $changed = $false
        foreach ($n in $nodes) {
            # Prefer Version attribute if present, otherwise add it
            if ($n.Attributes['Version']) {
                if ($n.Attributes['Version'].Value -ne $Version) {
                    $n.Attributes['Version'].Value = $Version
                    $changed = $true
                }
            } else {
                # If a child <Version> element exists, update it; else add Version attribute
                $childVer = $n.SelectSingleNode("*[local-name()='Version']")
                if ($childVer) {
                    if ($childVer.InnerText -ne $Version) {
                        $childVer.InnerText = $Version
                        $changed = $true
                    }
                } else {
                    $attr = $xml.CreateAttribute('Version')
                    $attr.Value = $Version
                    $null = $n.Attributes.Append($attr)
                    $changed = $true
                }
            }
        }

        if ($changed -and -not $WhatIf) {
            $xml.Save($Path)
        }

        return $changed
    } catch {
        Write-Warning "Failed to update csproj: $Path - $($_.Exception.Message)"
        return $false
    }
}

function Update-PackagesConfig {
    param(
        [Parameter(Mandatory=$true)][string]$Path,
        [Parameter(Mandatory=$true)][string]$Version
    )
    try {
        [xml]$xml = Get-Content -Raw -LiteralPath $Path
        $node = $xml.SelectSingleNode("//package[@id='Magick.NET-Q16-HDRI-AnyCPU']")
        if (-not $node) {
            return $false
        }

        if ($node.version -ne $Version) {
            $node.version = $Version
            if (-not $WhatIf) {
                $xml.Save($Path)
            }
            return $true
        }
        return $false
    } catch {
        Write-Warning "Failed to update packages.config: $Path - $($_.Exception.Message)"
        return $false
    }
}

$updated = 0
$scanned = 0

# Update any csproj using PackageReference
Get-ChildItem -LiteralPath $Root -Recurse -Filter *.csproj -ErrorAction SilentlyContinue | ForEach-Object {
    $scanned++
    if (Update-Csproj -Path $_.FullName -Version $Version) {
        $updated++
        Write-Host "UPDATED csproj: $($_.FullName)"
    } else {
        Write-Host "SKIPPED csproj: $($_.FullName)"
    }
}

# Update any packages.config (packages.config-based projects)
Get-ChildItem -LiteralPath $Root -Recurse -Filter packages.config -ErrorAction SilentlyContinue | ForEach-Object {
    $scanned++
    if (Update-PackagesConfig -Path $_.FullName -Version $Version) {
        $updated++
        Write-Host "UPDATED packages.config: $($_.FullName)"
    } else {
        Write-Host "SKIPPED packages.config: $($_.FullName)"
    }
}

Write-Host "Done. Scanned: $scanned, Updated: $updated"
if ($WhatIf) { Write-Host "Note: -WhatIf was used; no files were written." }