vanilla tweaks gui
-------

This GUI tool is used to modify the WoW.exe file of version 1.12,

These offset addresses are stolen from <https://github.com/brndd/vanilla-tweaks>

downloads : [vanilla-tweaks-gui.exe](https://github.com/R32/vanilla-tweaks/releases)

## Usage

1. Click `Open` to locate WoW.exe.

2. Adjust via the controls.

3. Click `Apply` to save changes to the file.

## Current patches

- Autoloot

  - `Origin` : Hold `shift` to loot automatically

  - `Reverse` : Hold `shift` to loot **manually**.

  - `Always` : Always loot automatically.

- Nameplate Distance : Adjust the distance of the nameplate to 20~41 yards

- FOV :

  - `Origin` : 1.5708

  - `Wide`   : 1.925

  - `Custom` : 2.1 (2.1 may work for ultrawide - adjust as needed)

- Camera Glitch :

  - `Origin` :

  - `Patch` :

## CHANGTES

- `0.3.1` :
  - Added Camera glitch patch, See #1
  - Changed `Save as` to `Apply`

- `0.3` : Added an input edit box for customizing the value of FOV, e.g. : `2.1` can be used for ultrawide.

- `0.2` : Added `en-us` resources for the English version



-------



香草微调(自动拾取修改器)
-------

此工具用于修改 1.12 版本的 WoW.exe 文件, 不是启动器, 也不修改内存

所修改的偏移地址来自于 <https://github.com/brndd/vanilla-tweaks> 的其中几个

工具(仅30KB)已打包好放在了[发布页面](https://github.com/R32/vanilla-tweaks/releases)

## 使用

1. 运行应用后, 点击 "打开" 按钮找到要修改的 WoW.exe 文件

2. 点击窗口左边的控件选择你想要的功能, 控件由 WoW.exe 文件的实际值而进行刷新

    > 如果控件不可用, 说明要修改的 WoW.exe 文件被别的工具修改过

3. 点击 "应用" 按钮保存修改到文件

## 功能

- 自动拾取

  - `原始` : 按下 `shift` 键将自动拾取

  - `反选` : 按下 `shift` 键将**手动拾取**, 网络上的自动拾取工具基本都属于这个类型

  - `总是` : 总是自动拾取

- 姓名版距离 : 调整姓名版的显示距离 20-41 码

- FOV :

  - `原始` : `1.5708`

  - `宽屏` : `1.925`

  - `自定义` : `2.1`, 可修改

- 镜头乱晃 :

  - `原始` :

  - `补丁` :

## 更新

- `0.3.1` :
  - 添加了镜头乱晃的修正补丁, 见 #1
  - 更改了 "另存为" 按钮为 "应用"

- `0.3` : 添加了一个输入框用于自定义 fov 的值, 例如 : `2.1` 可用于更宽的屏幕.

- `0.2` : 仅添加了英语资源用于老外
