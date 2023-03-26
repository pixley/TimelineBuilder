# ===================================
# Copyright (c) 2022 Tyler Pixley, all rights reserved.
# 
# This file (configure_qt_msvc.ps1) is part of TimelineBuilder's development toolset.
#
# TimelineBuilder is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later version.
#
# TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
# ===================================
# ===================================
# This script configures, builds, and installs Qt6 for use in TimelineBuilder builds via Visual Studio
# Requires Powershell 7 or later
# ===================================
param (
	[string]$target = "x64",
	[string]$platform = "win32`-msvc",
	[string]$build = "release",
	[int]$version = -1,
	[string]$win_sdk_version = "v10.0",
	[switch]$clean = $false,
	[switch]$skip_configure = $false,
	[switch]$skip_build = $false,
	[switch]$skip_install = $false,
	[switch]$debug = $false
)

# Reset the session's working directory and exit the script
function Finished
{
	param ([switch]$error = $false)
	
	# We changed our working directory, so now we return to the original.
	Pop-Location
	
	if ($error)
	{
		Exit -1
	}
	else
	{
		Exit 0
	}
}

# If Visual Studio is open, finish the script with an error
# Using the Ignore error action because Visual Studio **not** running is what we want.
if ((Get-Process devenv -ErrorAction Ignore) -ne $Null)
{
	Write-Error "Please close Visual Studio and try again."
	Finished -error
}
Write-Output "Visual Studio is not running.  Clear to proceed."

if ($PSVersionTable.PSVersion.Major -lt 7)
{
	Write-Error "Qt configuration script for TimelineBuilder requires PowerShell version 7 or later."
	Finished -error
}

# Cache the session's working directory so we can return to it later
Push-Location

# Navigate to the TimelineBuilder folder
if ($PSScriptRoot -ne $PWD)
{
	Set-Location $PSScriptRoot
	Write-Output "Directory set to TimelineBuilder root."
}
	
# If the Qt build folder doesn't exist yet, make it
$build_path = Join-Path $target $build extern qt6
$build_path_absolute = $PSScriptRoot + "\" + $build_path
$build_folder_exists = Test-Path -Path $build_path
if (-not $build_folder_exists)
{
	New-Item -Name $build_path -ItemType "directory"
	Write-Output "Created Qt6 build folder."
}

# Navigate to the Qt build folder
Set-Location $build_path

# If the clean_cmake flag is set, delete the CMake cache
if ($clean)
{
	$cache_file = $build_path_absolute + "\CMakeCache.txt"
	if (Test-Path -Path $cache_file -PathType leaf)
	{
		Remove-Item $cache_file
		Write-Output "Cleared CMake cache file."
	}
	else
	{
		Write-Output "Don't need to clean up the CMake cache file because there isn't one."
	}
}

# Prepare to launch Visual Studio Developer PowerShell
$version_arg = "`-latest"
if ($version -gt 0)
{
	$next_version = $version + 1
	$version_arg = "`-version `[$version`.0,$next_version`)"
}
$vs_where = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio" Installer vswhere.exe
$instance_id = & $vs_where $version_arg -property instanceId
$vs_path = & $vs_where $version_arg -property installationPath
$vs_year = & $vs_where $version_arg -property catalog.productLineVersion
$vs_version = & $vs_where $version_arg -property installationVersion
$dev_ps_launcher = Join-Path $vs_path Common7 Tools Launch-VsDevShell.ps1

if ($debug)
{
	Write-Output "Visual Studio install path: $vs_path"
	Write-Output "Visual Studio edition: $vs_year"
	Write-Output "Visual Studio version: $vs_version"
}

# Handle differences between versions of Dev PowerShell
if ($vs_year -eq "2019")
{
	# Prior to VS2022, the Dev PowerShell launch script did not take any relevant parameters
	& $dev_ps_launcher

	# Switch to using 64-bit compiler and libs (because the VS2019 Dev PowerShell is in 32-bit by default)
	# Requires some environment variable shenanigans to accomplish this
	# Basically, we're going to swap some paths from the x86 versions to the x64 versions
	# Fortunately, these changes only persist until the PowerShell session ends.
	
	# Starting with PATH
	# Despite the fact that Windows paths are case-insensitive, normal string replacement is.
	# We're using the entire path so that our replacement for cl.exe is laser-targeted
	$full_cl_path = Get-Command cl -syntax
	$cl_path_32 = $full_cl_path -ireplace [regex]::Escape("\cl.exe"), ""
	$cl_path_64 = $full_cl_path -ireplace [regex]::Escape("Hostx86\x86\cl.exe"), "Hostx64\x64"
	
	# We also need to do the replacement for the Windows Kit libraries
	# We can find our current version via the registry
	# Same laser-targeting applies here
	$win_kit_reg_vars = Get-Item "hklm:\SOFTWARE\WOW6432Node\Microsoft\Microsoft SDKs\Windows\$win_sdk_version"
	# The ".0" is added to the Windows Kit version string because it's part of the path, but not in the registry
	$win_kit_version_full = $win_kit_reg_vars.GetValue("ProductVersion") + ".0"
	if ($debug)
	{
		Write-Output "Found Windows Kit version: $win_kit_version_full"
	}
	$win_kit_base_path = $win_kit_reg_vars.GetValue("InstallationFolder")
	if ($debug)
	{
		Write-Output "Windows Kit root path: $win_kit_base_path"
	}
	$win_kit_bin_path = Join-Path $win_kit_base_path bin
	$win_kit_full_path = Join-Path $win_kit_bin_path $win_kit_version_full
	$win_kit_base_32 = Join-Path $win_kit_bin_path x86
	$win_kit_base_64 = Join-Path $win_kit_bin_path x64
	$win_kit_full_32 = Join-Path $win_kit_full_path x86
	$win_kit_full_64 = Join-Path $win_kit_full_path x64
	
	# Do the PATH replacements
	$path_string = (Get-Item env:path).Value
	$path_string = $path_string -ireplace [regex]::Escape($cl_path_32), $cl_path_64
	$path_string = $path_string -ireplace [regex]::Escape($win_kit_base_32), $win_kit_base_64
	$path_string = $path_string -ireplace [regex]::Escape($win_kit_full_32), $win_kit_full_64
	# Apply the replacements
	Set-Item -Path env:path -Value $path_string
	
	# And now we get to do the same stuff for the library env-vars, LIB and LIBPATH!
	# Fortunately, the INCLUDE env-var, despite being very similar, does not have platform-reliance
	# Same laser-focused path replacement as for PATH
	# We already have the path to cl.exe, so we can just use that to find the MSVC libs
	$msvc_path = $full_cl_path -ireplace [regex]::Escape("\bin\Hostx86\x86\cl.exe"), ""
	$msvc_lib_path_32 = Join-Path $msvc_path lib x86
	$msvc_lib_path_64 = Join-Path $msvc_path lib x64
	$msvc_ref_path_32 = Join-Path $msvc_lib_path_32 store references
	$msvc_ref_path_64 = Join-Path $msvc_lib_path_64 store references
	$msvc_atlmfc_path_32 = Join-Path $msvc_path ATLMFC lib x86
	$msvc_atlmfc_path_64 = Join-Path $msvc_path ATLMFC lib x64
	
	# Now for the Windows Kit stuff, whose base path and version we also already have
	$win_kit_lib_base = Join-Path $win_kit_base_path lib $win_kit_version_full
	$win_kit_um_32 = Join-Path $win_kit_lib_base um x86
	$win_kit_um_64 = Join-Path $win_kit_lib_base um x64
	$win_kit_ucrt_32 = Join-Path $win_kit_lib_base ucrt x86
	$win_kit_ucrt_64 = Join-Path $win_kit_lib_base ucrt x64
	
	# Do the LIB replacements
	$lib_string = (Get-Item env:lib).Value
	$lib_string = $lib_string -ireplace [regex]::Escape($msvc_lib_path_32), $msvc_lib_path_64
	$lib_string = $lib_string -ireplace [regex]::Escape($msvc_atlmfc_path_32), $msvc_atlmfc_path_64
	$lib_string = $lib_string -ireplace [regex]::Escape($win_kit_um_32), $win_kit_um_64
	$lib_string = $lib_string -ireplace [regex]::Escape($win_kit_ucrt_32), $win_kit_ucrt_64
	# Apply the replacements
	Set-Item -Path env:lib -Value $lib_string
	
	# Do the LIBPATH replacements
	$libpath_string = (Get-Item env:libpath).Value
	$libpath_string = $libpath_string -ireplace [regex]::Escape($msvc_lib_path_32), $msvc_lib_path_64
	$libpath_string = $libpath_string -ireplace [regex]::Escape($msvc_ref_path_32), $msvc_ref_path_64
	$libpath_string = $libpath_string -ireplace [regex]::Escape($msvc_atlmfc_path_32), $msvc_atlmfc_path_64
	# Apply the replacements
	Set-Item -Path env:libpath -Value $libpath_string
	
	# At this point, the VS2019 dev PowerShell should behave as though it was meant to do things in x64
}
elseif ($vs_year -eq "2022")
{
	# Launch Dev PowerShell in 64-bit mode, because that's a thing we can do now
	& $dev_ps_launcher -Arch amd64
}
else
{
	Write-Error "Could not find a supported version of Visual Studio!"
	Finished -error
}

if ($debug)
{
	Write-Output "Checking MSVC compiler."
	# Get-Command cl.exe -Syntax
	# Calling cl.exe with no parameters gives us version info
	cl
}

# I'm not entirely sure what this does, but it was in the example I followed, and not having it breaks things
set CL=/MP

if (-not $skip_configure)
{
	# Run Qt's build configuration script.
	# For some reason, the official instructions recommend running it from the destination directory
	Write-Output "Starting Qt build configuration."
	$configure_bat = Join-Path $PSScriptRoot extern qt6 configure.bat
	# Note: despite TimelineBuilder using C++20, we build Qt with C++17 due to build-breaking coroutine changes
	& $configure_bat -prefix $build_path_absolute "-$build" -static -c++std c++17 -platform $platform
	# Check to see if configuration failed
	if ($LastExitCode -ne 0)
	{
		Write-Error "Configure.bat failed!  Terminating script!"
		if ($debug)
		{
			Write-Warning "Exit code: $LastExitCode"
		}
		Finished -error
	}
	Write-Output "Qt build configuration complete."

	# configure.bat changes our working directory, so change it back to the Qt build folder
	Set-Location $build_path_absolute
}

if (-not $skip_build)
{
	# Build Qt via CMake
	Write-Output "Starting build via CMake."
	$build_args = "--build . --parallel"
	if ($clean)
	{
		$build_args = $build_args + " --clean-first"
	}
	Start-Process -FilePath "cmake" -ArgumentList $build_args -NoNewWindow -Wait
	# Check to see if build failed
	if ($LastExitCode -ne 0)
	{
		Write-Error "CMake build failed!  Terminating script!"
		if ($debug)
		{
			Write-Warning "Exit code: $LastExitCode"
		}
		Finished -error
	}
	Write-Output "Qt has been successfully built."
}

if (-not $skip_install)
{
	# Install Qt via CMake
	Write-Output "Starting installation via CMake."
	Start-Process -FilePath "cmake" -ArgumentList "--install ." -NoNewWindow -Wait
	# Check to see if installation failed
	if ($LastExitCode -ne 0)
	{
		Write-Error("CMake installation failed!  Terminating script!")
		if ($debug)
		{
			Write-Warning "Exit code: $LastExitCode"
		}
		Finished -error
	}
	Write-Output "Qt has been successfully installed."
}

Write-Output "Qt is ready for use with TimelineBuilder!"
Finished