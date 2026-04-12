$path = "C:\Users\chowd\Documents\GitHub\QuickGIS\src\app\qgsstatusbarcoordinateswidget.cpp"
$text = [System.IO.File]::ReadAllText($path)

# The duplication likely occurred when I appended the math functions to a file that already had them.
# The appended section starts from the first '//Nihcas below Function for Mil Grid' that is AFTER the end of the original file,
# or more specifically, my script inserted:
# $mathFunctions + "`r`n`r`n" + $updateExtra + "`r`n`r`n" + $insertionPoint

# Let's find the first instance of 'void QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged()'
$idx1 = $text.IndexOf("void QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged()")
if ($idx1 -ge 0) {
    # Let's find the second instance
    $idx2 = $text.IndexOf("void QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged()", $idx1 + 10)
    
    if ($idx2 -ge 0) {
        # The file has duplication. The text from the beginning up until just before the second instance 
        # (or where the duplicate math functions start) needs to be kept.
        # Actually, in my edit.ps1 I did:
        # $newText = $parts[0] + $mathFunctions + "\n\n" + $updateExtra + "\n\n" + $insertionPoint + $parts[1]
        # $parts[0] ended exactly at the FIRST 'void QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged()'
        # So I literally injected $mathFunctions BEFORE the first instance.
        
        # Did the user's file already have the math functions? 
        # If it did, it would be in $parts[1] or $parts[0].
    }
}

# Instead of complex logic, why not restore the file from git (if it was tracked and safe), 
# OR just find the exact string we injected and REPLACE it with empty string?

$src = "C:\Users\chowd\Documents\GitHub\surveykshak\src\app\qgsstatusbarcoordinateswidget.cpp"
$srcText = [System.IO.File]::ReadAllText($src)
$sIdx = $srcText.IndexOf("//Nihcas below Function for Mil Grid")
$eIdx = $srcText.IndexOf("//Nihcas above", $sIdx) + "//Nihcas above".Length
$mathFunctions = $srcText.Substring($sIdx, $eIdx - $sIdx)

$startU = $srcText.IndexOf("//Overload updatecordinate")
$endU = $srcText.IndexOf("void QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged()")
$updateExtra = $srcText.Substring($startU, $endU - $startU)
$updateExtra = $updateExtra.Replace('QgsCoordinateReferenceSystem(4326)', 'QgsCoordinateReferenceSystem("EPSG:4326")')

$insertionPoint = "void QgsStatusBarCoordinatesWidget::coordinateDisplaySettingsChanged()"

$injectedString = $mathFunctions + "`r`n`r`n" + $updateExtra + "`r`n`r`n"

# Remove the exact injected string
if ($text.Contains($injectedString)) {
    $fixedText = $text.Replace($injectedString, "")
    [System.IO.File]::WriteAllText($path, $fixedText)
    Write-Host "Fixed large duplication."
} else {
    # Maybe newline differences (`n instead of `r`n)
    $injectedStringLF = $mathFunctions.Replace("`r`n", "`n") + "`n`n" + $updateExtra.Replace("`r`n", "`n") + "`n`n"
    $textLF = $text.Replace("`r`n", "`n")
    if ($textLF.Contains($injectedStringLF)) {
        $fixedText = $textLF.Replace($injectedStringLF, "")
        [System.IO.File]::WriteAllText($path, $fixedText)
        Write-Host "Fixed large duplication (LF)."
    } else {
        Write-Host "Could not find exact injected string. Will use regex to remove second definitions."
        # Backup the current file just in case
        Copy-Item $path "$path.bak"
        
        # If the file contains two 'QString QgsStatusBarCoordinatesWidget::LatLongToMilgridConversion',
        # we can find the second occurrence and delete everything from it to the end, assuming it's at the end.
    }
}
