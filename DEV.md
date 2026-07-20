# Make a new release

## Steps

### 1. Update `README.txt`
Update the version number on line 3:
```
Version X.Y.Z
```

### 2. Update `distrib/fmit.appdata.xml`
Add a new `<release>` entry at the top of the `<releases>` section (after line 272):
```xml
<release version="X.Y.Z" date="YYYY-MM-DD">
  <description>
    <ul>
      <li>Change description</li>
      <li xml:lang="de">Änderung Beschreibung</li>
      <li xml:lang="el">Περιγραφή αλλαγής</li>
      <li xml:lang="fr">Description du changement</li>
      <li xml:lang="nb-NO">Endringsbeskrivelse</li>
      <li xml:lang="pt">Descrição da mudança</li>
      <li xml:lang="pt-BR">Descrição da mudança</li>
      <li xml:lang="ru">Описание изменений</li>
      <li xml:lang="ta">மாற்ற விவரణை</li>
      <li xml:lang="zh-HANS">更改说明</li>
    </ul>
  </description>
</release>
```

### 3. Commit the changes
```bash
git add README.txt distrib/fmit.appdata.xml
git commit -m "vX.Y.Z"
```

### 4. Create and push the git tag
```bash
git tag -a vX.Y.Z -m "vX.Y.Z"
git push origin vX.Y.Z
```

### 5. Wait for GitHub Actions
The `build.yml` workflow triggers on tags (`v*`):
- Builds Linux (.deb), Windows (.exe) packages
- Creates a **draft GitHub Release** with all artifacts

### 6. Publish the release
- Go to GitHub → Releases
- Review the draft release
- Publish when ready
