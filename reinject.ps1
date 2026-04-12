$src = "C:\Users\chowd\Documents\GitHub\surveykshak\src\app\qgsstatusbarcoordinateswidget.cpp"
$dst = "C:\Users\chowd\Documents\GitHub\QuickGIS\src\app\qgsstatusbarcoordinateswidget.cpp"

$srcText = [System.IO.File]::ReadAllText($src)
$sIdx = $srcText.IndexOf("//Nihcas below Function for Mil Grid")
$eIdx = $srcText.IndexOf("//Nihcas above", $sIdx) + "//Nihcas above".Length
$mathFunctions = $srcText.Substring($sIdx, $eIdx - $sIdx)

$startU = $srcText.IndexOf("//Overload updatecordinate")
$endU = $srcText.IndexOf("void QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged()")
$updateExtra = $srcText.Substring($startU, $endU - $startU)
$updateExtra = $updateExtra.Replace('QgsCoordinateReferenceSystem(4326)', 'QgsCoordinateReferenceSystem("EPSG:4326")')

$dstText = [System.IO.File]::ReadAllText($dst)

$insertionPoint = "void QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged()"
$parts = $dstText.Split([string[]]@($insertionPoint), [System.StringSplitOptions]::None)

# Assert that parts is 2 elements so we don't duplicate or fail
if ($parts.Length -eq 2) {
    if (-not $parts[0].Contains("LatLongToMilgridConversion")) {
        $newText = $parts[0] + $mathFunctions + "`r`n`r`n" + $updateExtra + "`r`n`r`n" + $insertionPoint + $parts[1]
        [System.IO.File]::WriteAllText($dst, $newText)
        Write-Host "Injected properly."
    } else {
        Write-Host "Wait, it's still in the file? Aborting to prevent duplicates."
    }
} else {
    Write-Host "Insertion point found $($parts.Length - 1) times. Cannot safely inject."
}
