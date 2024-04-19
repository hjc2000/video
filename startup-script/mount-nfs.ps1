# 获取所有当前已经映射的驱动器
$usedDrives = Get-PSDrive -PSProvider 'FileSystem' | Select-Object -ExpandProperty Name

# 定义一个数组，包含所有可能的驱动器字母
$allDrives = 65..90 | ForEach-Object { [char]$_ + ":" }

# 找出第一个未使用的驱动器
$freeDrive = $allDrives | Where-Object { $usedDrives -notcontains $_ } | Select-Object -First 1

if (-not $freeDrive)
{
	Write-Host "No free drive letters found."
	exit
}

# NFS 服务器地址
$nfsServer = "192.168.8.8"
# NFS 共享路径
$nfsPath = "/mnt/nvme0n1p3"
# 完整的 UNC 路径
$uncPath = "\\$nfsServer$nfsPath"

# 使用 New-PSDrive 挂载 NFS 共享
New-PSDrive -Name $freeDrive.Trim(':') -PSProvider FileSystem -Root $uncPath -Persist -Scope Global

Write-Host "NFS share mounted on $freeDrive"
