$path = "C:\Users\chowd\Documents\GitHub\QuickGIS\src\app\qgsstatusbarcoordinateswidget.cpp"
$text = [System.IO.File]::ReadAllText($path)

# 1. Deduplicate Widgets in Constructor
# I noticed from `view_file` (lines 61-155) that both labels and line edits are defined twice.
# Let's find the first instance of 'mLabelgeocord = new QLabel' up to line 307.

# A more robust way: Find the first QHBoxLayout creation and remove EVERYTHING from there to the end of the constructor,
# then reinject the layout part correctly.

# 2. Deduplicate Math Functions
# Find "LatLongToMilgridConversion" and see how many times it exists.
$count = ([regex]::Matches($text, "QgsStatusBarCoordinatesWidget::LatLongToMilgridConversion")).Count
Write-Host "Found $count definitions of LatLongToMilgridConversion"

if ($count -gt 1) {
    # We have duplicates. We need to find the SECOND one and delete from there to the next definition? 
    # Or just find the first '//Nihcas below Function for Mil Grid' and delete everything before it that is ALSO in the injected block?
    
    # Actually, the most reliable way is to restore the file to a clean state if possible, 
    # but I don't have a backup.
}

# Let's try to remove labels and line edits duplication first.
# From Step 402:
# 61: mLabelgeocord = ...
# 112: mLabelgeocord = ... (DUPLICATION)

# 167: mCoordsGeocord = ...
# 221: mCoordsGeocord = ... (DUPLICATION)

$newText = $text
$dupLabelStart = "  mLabelgeocord = new QLabel(QString(), this);"
$idxFirstLabel = $newText.IndexOf($dupLabelStart)
if ($idxFirstLabel -ge 0) {
    $idxSecondLabel = $newText.IndexOf($dupLabelStart, $idxFirstLabel + 10)
    if ($idxSecondLabel -ge 0) {
        # The duplicate block of labels ends just before 'mLineEdit = new QLineEdit'
        $endOfDupLabels = $newText.IndexOf("  mLineEdit = new QLineEdit( this );", $idxSecondLabel)
        if ($endOfDupLabels -ge 0) {
             # Remove from $idxSecondLabel to $endOfDupLabels
             $newText = $newText.Remove($idxSecondLabel, $endOfDupLabels - $idxSecondLabel)
             Write-Host "Removed duplicate labels."
        }
    }
}

$dupLineEditStart = "  mCoordsGeocord = new QLineEdit(this);"
$idxFirstLE = $newText.IndexOf($dupLineEditStart)
if ($idxFirstLE -ge 0) {
    $idxSecondLE = $newText.IndexOf($dupLineEditStart, $idxFirstLE + 10)
    if ($idxSecondLE -ge 0) {
        # The duplicate block of line edits ends just before 'mToggleExtentsViewButton = new QToolButton'
        $endOfDupLEs = $newText.IndexOf("  //toggle to switch between mouse pos and extents display in status bar widget", $idxSecondLE)
        if ($endOfDupLEs -ge 0) {
             # Remove from $idxSecondLE to $endOfDupLEs
             $newText = $newText.Remove($idxSecondLE, $endOfDupLEs - $idxSecondLE)
             Write-Host "Removed duplicate line edits."
        }
    }
}

# Now for the functions. 
# Search for ALL definitions and keep only the first one of each? 
# No, let's locate the entire `//Nihcas` block.
$startMarker = "//Nihcas below Function for Mil Grid"
$endMarker = "//Nihcas above"

$sIdx1 = $newText.IndexOf($startMarker)
if ($sIdx1 -ge 0) {
    $sIdx2 = $newText.IndexOf($startMarker, $sIdx1 + 10)
    if ($sIdx2 -ge 0) {
        # We have a duplicate block. Let's remove the second one.
        $eIdx2 = $newText.IndexOf($endMarker, $sIdx2) + $endMarker.Length
        if ($eIdx2 -gt $sIdx2) {
             $newText = $newText.Remove($sIdx2, $eIdx2 - $sIdx2)
             Write-Host "Removed duplicate Nihcas functions block."
        }
    }
}

[System.IO.File]::WriteAllText($path, $newText)
Write-Host "Done fixing."
