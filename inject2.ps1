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
$idx = $dstText.IndexOf($insertionPoint)

if ($idx -ge 0) {
    if (-not $dstText.Contains("LatLongToMilgridConversion(const QgsPointXY& mp)")) {
        $prefix = $dstText.Substring(0, $idx)
        $suffix = $dstText.Substring($idx)
        
        $newText = $prefix + $mathFunctions + "`r`n`r`n" + $updateExtra + "`r`n`r`n" + $suffix
        [System.IO.File]::WriteAllText($dst, $newText, [System.Text.Encoding]::UTF8)
        Write-Host "Injected exactly once."
    } else {
        Write-Host "Method already exists in file. Not injecting."
    }
} else {
    Write-Host "Insertion point not found."
}
