$path = "C:\Users\chowd\Documents\GitHub\QuickGIS\src\app\qgsstatusbarcoordinateswidget.cpp"
$text = [System.IO.File]::ReadAllText($path)
$lines = [System.IO.File]::ReadAllLines($path)

$functions = @(
    "QString QgsStatusBarCoordinatesWidget::eveLatLongTopoSheetConversion",
    "QString QgsStatusBarCoordinatesWidget::LatLongTopoSheetConversion",
    "QString QgsStatusBarCoordinatesWidget::LatLongToMilgridConversion",
    "QString QgsStatusBarCoordinatesWidget::eveLatLongToMilgridConversion",
    "const char* QgsStatusBarCoordinatesWidget::check_row_2_sides",
    "int QgsStatusBarCoordinatesWidget::check_domain_2_sides",
    "const char* QgsStatusBarCoordinatesWidget::check_row_3_sides",
    "int QgsStatusBarCoordinatesWidget::check_domain_3_sides",
    "const char* QgsStatusBarCoordinatesWidget::check_row_4_sides",
    "int QgsStatusBarCoordinatesWidget::check_domain_4_sides",
    "int* QgsStatusBarCoordinatesWidget::checkarray",
    "void QgsStatusBarCoordinatesWidget::updateCoordinateDisplayUpdated"
)

$seen = @{}
$finalLines = New-Object System.Collections.Generic.List[string]
$skipping = $false
$braceCount = 0

foreach ($line in $lines) {
    if (-not $skipping) {
        $foundFunc = $null
        foreach ($func in $functions) {
            if ($line.Contains($func)) {
                if ($seen.ContainsKey($func)) {
                    $skipping = $true
                    $braceCount = 0
                    Write-Host "Found duplicate of $func. Skipping..."
                    break
                } else {
                    $seen[$func] = $true
                    Write-Host "Kept first instance of $func."
                }
            }
        }
        
        if (-not $skipping) {
            $finalLines.Add($line)
        }
    }
    
    if ($skipping) {
        # Track braces to find the end of the duplicate function body
        $braceCount += ($line.ToCharArray() | Where-Object { $_ -eq '{' }).Count
        $braceCount -= ($line.ToCharArray() | Where-Object { $_ -eq '}' }).Count
        
        if ($braceCount -eq 0 -and $line.Contains("}")) {
            $skipping = $false
        }
    }
}

[System.IO.File]::WriteAllLines($path, $finalLines, [System.Text.Encoding]::UTF8)
Write-Host "Done deduplicating functions."
