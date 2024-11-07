# TappyFowl

![Unreal Engine Version](https://img.shields.io/badge/Unreal%20Engine-5.4-000000?style=for-the-badge)
![Thirdweb Plugin Version](https://img.shields.io/badge/ThirdwebSDK-1.7.0--dev-000000?style=for-the-badge)


> [!NOTE]  
> Readme is a work in progress

## Development Setup

### Prerequisites
1. Visual Studio 2022 LTSC 17.10
   1. Download the setup executable from https://visualstudio.microsoft.com/vs/
   2. Run the setup, changing the channelUri as shown below
      ```powershell
      visualstudiosetup.exe --channelUri https://aka.ms/vs/17/release.LTSC.17.10/channel
      ```
   3. Select the following **Workloads**:
      * .NET desktop development
      * Desktop development with C++
      * .NET Multi-platform App UI development
      * Game development with C++. You will need to add these additional options:
         * C++ profiling tools
         * C++ AddressSanitizer
         * Windows 10 SDK (10.0.18362 or Newer)
         * Unreal Engine installer
   4. Select the following **Individual Components**:
      * MSVC v143 - VS 2022 C++ x64/x86 build tools (v14.38-17.8)
   5. Install
2. Unreal Engine 5.4 
   1. Install the Epic Launcher from https://store.epicgames.com/en-US/download
   2. Open the epic launcher and navigate to `Unreal Engine > Library > Engine Versions`
   3. Click `+` Next to `ENGINE VERSIONS`
   4. Select 5.4.x
   5. Click Install
3. LE Extended Standard Library Plugin (free)
   1. add it to your library https://www.fab.com/listings/0aadd41b-c02d-4f63-9009-bffad0070ebc
   2. Open the epic launcher and navigate to `Unreal Engine > Library > Fab Library`
   3. Find the plugin and click `Install to Engine`
   4. Select 5.4 and click `Install`

### Cloning
1. Clone the project
   ```powershell
   git clone git@github.com:thirdweb-example/TappyFowl.git C:\Projects\UnrealEngine\TappyFowl
   ```
2. Create a Plugins folder inside the project
   ```powershell
   mkdir C:\Projects\UnrealEngine\TappyFowl\Plugins
   ```
3. Create a Plugins folder inside the project
   ```powershell
   mkdir C:\Projects\UnrealEngine\TappyFowl\Plugins
   ```
4. Clone the ThirdwebSDK into your plugins folder
   > [!NOTE]
   > If the current sdk version above is not -dev, you can also just install it from the marketplace
   ```powershell
   git clone git@github.com:thirdweb-dev/unreal-sdk.git C:\Projects\UnrealEngine\TappyFowl\Plugins\ThirdwebSDK
   ```

### Generating Project Files
1. Open File Explorer and navigate to the project directory
2. Shift+Right click `TappyFowl.uproject`
3. Click `Switch Unreal Engine Version...`
4. Select `5.4`
5. Click `OK`

### Building The Editor
1. Open `TappyFowl.sln` in either Visual Studio or Rider
2. In the `Solution Explorer` on the right - under `Games` - Right click `TappyFowl`
3. Click `Build`
4. Wait for build to complete
5. Start the editor by either pressing CTRL+F5 or by pressing the small green triangle button on the top bar
(When hovering it should say `Start without debugging`)
