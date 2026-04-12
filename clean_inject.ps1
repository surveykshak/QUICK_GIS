$path = "C:\Users\chowd\Documents\GitHub\QuickGIS\src\app\qgsstatusbarcoordinateswidget.cpp"
$text = [System.IO.File]::ReadAllText($path)

$startMarker = "//Nihcas below Function for Mil Grid"
$endMarker = "//Nihcas above"

# Find the very first start marker and the very last end marker
$firstStart = $text.IndexOf($startMarker)
$lastEnd = $text.LastIndexOf($endMarker)

if ($firstStart -ge 0 -and $lastEnd -gt $firstStart) {
    # Remove everything between them (inclusive)
    $before = $text.Substring(0, $firstStart)
    $after = $text.Substring($lastEnd + $endMarker.Length)
    $cleanText = $before + $after
    Write-Host "Wiped out existing Nihcas blocks."
} else {
    $cleanText = $text
    Write-Host "No Nihcas blocks found to wipe."
}

# Now reinject once
$src = "C:\Users\chowd\Documents\GitHub\surveykshak\src\app\qgsstatusbarcoordinateswidget.cpp"
$srcText = [System.IO.File]::ReadAllText($src)

$sIdx = $srcText.IndexOf($startMarker)
$eIdx = $srcText.IndexOf($endMarker, $sIdx) + $endMarker.Length
$mathFunctions = $srcText.Substring($sIdx, $eIdx - $sIdx)

$startU = $srcText.IndexOf("//Overload updatecordinate")
$endU = $srcText.IndexOf("void QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged()")
$updateExtra = $srcText.Substring($startU, $endU - $startU)
$updateExtra = $updateExtra.Replace('QgsCoordinateReferenceSystem(4326)', 'QgsCoordinateReferenceSystem("EPSG:4326")')

$insertionPoint = "void QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged()"
$idx = $cleanText.IndexOf($insertionPoint)

if ($idx -ge 0) {
    $prefix = $cleanText.Substring(0, $idx)
    $suffix = $cleanText.Substring($idx)
    $finalText = $prefix + $mathFunctions + "`r`n`r`n" + $updateExtra + "`r`n`r`n" + $suffix
    [System.IO.File]::WriteAllText($path, $finalText, [System.Text.Encoding]::UTF8)
    Write-Host "Re-injected exactly once."
} else {
    Write-Host "Could not find insertion point in cleaned text."
}
