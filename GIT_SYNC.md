# Syncing QUICK_GIS with Official QGIS

This guide explains the workflow to keep your custom **QUICK_GIS** repository up-to-date with official QGIS patches and updates (such as the `release-3_44` branch).

## Prerequisites

Before you can sync, you need to tell your local Git repository where the official QGIS repository is located. We do this by adding an "upstream" remote.

1. **Open your terminal/command prompt** in your `QUICK_GIS` directory.
2. **Check your current remotes:**
   ```bash
   git remote -v
   ```
   *You should see `origin` pointing to your QUICK_GIS repository.*

3. **Add the official QGIS upstream remote** (you only need to do this once):
   ```bash
   git remote add upstream https://github.com/qgis/QGIS.git
   ```

4. **Verify the new remote:**
   ```bash
   git remote -v
   ```
   *You should now see both `origin` (your QUICK_GIS) and `upstream` (the official QGIS repo).*

---

## Syncing Workflow (e.g., with `release-3_44`)

When official QGIS releases patches to a branch like `release-3_44`, follow these steps to pull those changes into your custom QUICK_GIS features.

### 1. Fetch Upstream Changes

Download the latest commits and branches from the official QGIS repository without modifying your local working files yet:

```bash
git fetch upstream
```

### 2. Checkout Your Working Branch

Switch to the local branch in your QUICK_GIS repository that you want to update. This might be your main development branch or a branch specifically tracking `3.44`.

```bash
git checkout <your-custom-branch>
# Example: git checkout main
```

### 3. Integrate Upstream Changes (Merge vs. Rebase)

You have two main options to integrate the official changes. **Merging** is generally safer and recommended if multiple people are working on your QUICK_GIS branch. **Rebasing** creates a cleaner history but should only be used if you are the only one working on the branch or are comfortable force-pushing.

#### Option A: Merge (Recommended)
This creates a new "merge commit", preserving the exact history of both your custom features and the official patches.

```bash
git merge upstream/release-3_44
```

#### Option B: Rebase
This rewrites your local history by placing all of your custom QUICK_GIS commits *on top* of the newly fetched official QGIS commits.

```bash
git rebase upstream/release-3_44
```

### 4. Resolve Conflicts (If Any)

Because you have custom QUICK_GIS features, there is a high chance of merge conflicts if official QGIS modified the same files as your customizations.

1. Git will pause the merge/rebase and list the files with conflicts.
2. Open those files in your code editor. Look for the conflict markers:
   ```text
   <<<<<<< HEAD
   // Your custom QUICK_GIS code
   =======
   // Official QGIS patch code
   >>>>>>> upstream/release-3_44
   ```
3. Carefully edit the file to incorporate both the official patch and your custom logic. Remove the conflict markers.
4. Stage the resolved files:
   ```bash
   git add <resolved-file>
   ```
5. **Finalize the integration:**
   - If merging: Run `git commit` to finalize the merge commit.
   - If rebasing: Run `git rebase --continue` to proceed to the next commit.

### 5. Push to QUICK_GIS Origin

Once the integration is complete and you have verified that QUICK_GIS still builds and runs correctly, push your updated branch to your repository:

```bash
git push origin <your-custom-branch>
```

> **Note:** If you used **rebase** in step 3, you will likely need to force push:
> `git push -f origin <your-custom-branch>`
> *(Use force push with caution if sharing branches with other developers!)*
-----------------------------------------------
git status; git remote -v; git branch --show-current
git fetch upstream
git merge upstream/release-3_44
git push origin release-3_44
------------------------------------------------
