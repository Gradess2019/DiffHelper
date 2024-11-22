# Diff Helper

## Description
The Diff Helper plugin introduces a new window in the Unreal Engine editor, enabling you to view diff between two Git branches. It is especially useful when you develop projects under Git version control and need to review changes made by your teammates easily.

## How to install manually
1. Download [latest release](https://github.com/Gradess2019/DiffHelper/releases/latest) for your Unreal Engine version
2. Unzip into: **\<ProjectDirectory\>/Plugins** (create Plugins directory if it doesn't exist)
3. If you are using C++: Right Mouse Button on your **.uproject** file -> Generate Visual Studio project files
4. Launch project
5. If it's not enabled: Go to Edit -> Plugins -> "Project" category -> Editor -> Enable "Diff Helper" and restart the editor
7. Done

## How to use
1. To start, click on **Tools** in the top menu, then navigate to **Revision Control** and select **Diff Helper**.
![image](https://github.com/user-attachments/assets/59d7d3ea-7858-43a1-a06a-c8773a502996)
2. A new window with a tab labeled **Revision Picker** will open. Select the target branch (first) and the source branch (second) from the dropdowns, then click the **Show Diff** button. Loading may take a few moments.
![image](https://github.com/user-attachments/assets/272662c9-36c9-442f-a13a-ac603ac4c89f)
3. Once the diff is loaded, a new tab will appear, organized as follows:
    1. **Diff Panel**: Displays all affected files.
    2. **Commit Panel**: Shows commits related to the selected file in the Diff Panel.
    3. **Diff Panel Toolbar**:
        - **Open Asset**: Opens the selected asset in the editor (if possible).
        - **Show in Content Browser**: Locates the selected file in the Content Browser (if possible).
        - **Show in Explorer**: Opens the selected file in Explorer (if possible).
        - **Group by Directory**: Organizes files by directory, displaying them in a tree structure.
    4. **Commit Panel Toolbar**:
        - **Diff Against Target**: Compares the selected file in the target branch to the source branch.
        - **Diff Selected**: Compares two selected commits in the Commit Panel.
        - **Diff Selected Commit Against Next One**: Compares the selected commit with the next one in the sequence.
        - **Diff Selected Commit Against Previous One**
        - **Diff Selected Commit Against Newest**
        - **Diff Selected Commit Against Oldest**

    ![image](https://github.com/user-attachments/assets/616869aa-9fde-4221-a67d-2938dbc941e8)


## Configuration
To customize plugin settings, go to **Edit -> Editor Preferences -> Plugins -> Diff Helper**.
![image](https://github.com/user-attachments/assets/d21403af-581f-4b34-b912-7ec53736d437)
