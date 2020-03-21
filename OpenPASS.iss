;/*******************************************************************************
;* Copyright (c) 2020 HLRS, University of Stuttgart
;*
;* This program and the accompanying materials are made
;* available under the terms of the Eclipse Public License 2.0
;* which is available at https://www.eclipse.org/legal/epl-2.0/
;*
;* SPDX-License-Identifier: EPL-2.0
;*******************************************************************************/
                            
#define VERSIONSTRING GetDateTimeString('yyyy/mm/dd', '.', '');

#define OP_HOME GetEnv("OPENPASS_HOME")
#define OP_SOURCE GetEnv("OPENPASS_HOME")+"/../.."

; get environment variables for extern libs
#define QT GetEnv("EXTERNLIBS")+"\qt5"           
#define ICU GetEnv("EXTERNLIBS")+"\icu"
#define BOOST  GetEnv("EXTERNLIBS")+"\boost"
#define XERCES  GetEnv("EXTERNLIBS")+"\xerces"
#define OSI  GetEnv("EXTERNLIBS")+"\osi"
#define PROTOBUF  GetEnv("EXTERNLIBS")+"\Protobuf"
#define EXTERNLIBS GetEnv("EXTERNLIBS")

#define DIST OP_SOURCE+"\DIST"
#define BIN  OP_HOME+"\bin"
#define LIB  OP_HOME+"\lib"
#define DBIN "{app}\bin"
#define DLIB "{app}\lib"
#define DDOC "{app}\Documentation"
#define ICONFILE GetEnv("OPENPASS_HOME")+"\..\..\OpenPass_Source_Code\openPASS_GUI\openPASS\openPASS.ico" 
#define DICONFILE "{app}\bin\openPASS.ico"

#define SYS GetEnv("EXTERNLIBS")+"\runtime\*.*"

#define SUFFIX_VERSION GetDateTimeString('yyyy/mm/dd', '-', ':');

[Setup]

PrivilegesRequired=None
ArchitecturesInstallIn64BitMode="x64"

OutputDir={#DIST}
OutputBaseFilename=OpenPASS_{#SUFFIX_VERSION}

AppName=OpenPASS
AppVerName=OpenPASS {#VERSIONSTRING}

AppPublisher=HLRS
AppPublisherURL=http://www.hlrs.de
AppSupportURL=http://www.hlrs.de
AppUpdatesURL=http://www.hlrs.de
ChangesAssociations=false
ChangesEnvironment=true
DefaultDirName={reg:HKLM\SOFTWARE\OpenPASS,Path|{autopf}\OpenPASS}
DefaultGroupName={reg:HKLM\SOFTWARE\OpenPASS,StartMenu|OpenPASS}
DisableStartupPrompt=true

SetupIconFile={#ICONFILE}
ShowLanguageDialog=no

[Types]
Name: standard; Description: OpenPASS Standard Installation
Name: custom; Description: Custom Installation; Flags: iscustom
[Components]

Name: core; Description: OpenPASS core system; Types: standard custom
Name: help; Description: OpenPASS help and tutorial system; Types: standard custom
Name: example; Description: OpenPASS example data; Types: standard custom
Name: OSI; Description: OpenPASS OSI modules; Types: standard custom
Name: PCM; Description: OpenPASS PCM modules; Types: standard custom

[Files]

Source: {#OP_SOURCE}\OpenPass_Source_Code\openPASS_Resource\*; Flags: recursesubdirs; DestDir: {app}\openPASS_Resource; Components: example  
Source: {#OP_HOME}\configs\*; DestDir: {app}\configs; Flags: recursesubdirs; Components: core 
Source: {#OP_HOME}\components\*; DestDir: {app}\components; Flags: recursesubdirs; Components: core 
;Source: {#OP_HOME}\results; DestDir: {app}\results; Flags: recursesubdirs; Components: core 

Source: {#OP_HOME}\*.exe; DestDir: {app}; Components: core
Source: {#BIN}\*.lib; DestDir: {#DBIN}; Flags: recursesubdirs; Components: core
Source: {#BIN}\*.dll; DestDir: {#DBIN}; Flags: recursesubdirs; Components: core
Source: {#LIB}\*.dll; DestDir: {#DLIB}; Flags: recursesubdirs; Components: core
Source: {#LIB}\*.lib; DestDir: {#DLIB}; Flags: recursesubdirs; Components: core

Source: {#EXTERNLIBS}\runtime\bin\VC_redist.x64.exe; DestDir: {#DBIN}; Flags: recursesubdirs; Components: core

; we are using Qt version >= 4.5.0 LGPL  
Source: {#QT}\bin\Qt5Core.dll; DestDir: {#DLIB}; Components: core  
Source: {#QT}\bin\Qt5Xml.dll; DestDir: {#DLIB}; Components: core
Source: {#QT}\bin\Qt5Gui.dll; DestDir: {#DLIB}; Components: core
Source: {#QT}\bin\Qt5Widgets.dll; DestDir: {#DLIB}; Components: core
Source: {#QT}\bin\QtWebEngineProcess*.exe; DestDir: {#DBIN}; Components: core
  
Source: {#QT}\plugins\imageformats\qgif.dll; DestDir: {#DLIB}\plugins\imageformats; Flags: recursesubdirs; Components: core
Source: {#QT}\plugins\imageformats\qjpeg.dll; DestDir: {#DLIB}\plugins\imageformats; Flags: recursesubdirs; Components: core
Source: {#QT}\plugins\imageformats\qsvg.dll; DestDir: {#DLIB}\plugins\imageformats; Flags: recursesubdirs; Components: core  
Source: {#QT}\plugins\imageformats\qtiff.dll; DestDir: {#DLIB}\plugins\imageformats; Flags: recursesubdirs; Components: core
Source: {#QT}\plugins\imageformats\qico.dll; DestDir: {#DLIB}\plugins\imageformats; Flags: recursesubdirs; Components: core
Source: {#QT}\plugins\imageformats\qwbmp.dll; DestDir: {#DLIB}\plugins\imageformats; Flags: recursesubdirs; Components: core
  
Source: {#QT}\plugins\platforms\qdirect2d.dll; DestDir: {#DLIB}\plugins\platforms; Flags: recursesubdirs; Components: core
Source: {#QT}\plugins\platforms\qminimal.dll; DestDir: {#DLIB}\plugins\platforms; Flags: recursesubdirs; Components: core
Source: {#QT}\plugins\platforms\qoffscreen.dll; DestDir: {#DLIB}\plugins\platforms; Flags: recursesubdirs; Components: core   
Source: {#QT}\plugins\platforms\qwebgl.dll; DestDir: {#DLIB}\plugins\platforms; Flags: recursesubdirs; Components: core
Source: {#QT}\plugins\platforms\qwindows.dll; DestDir: {#DLIB}\plugins\platforms; Flags: recursesubdirs; Components: core    
 
Source: {#QT}\plugins\platformthemes\qflatpak.dll; DestDir: {#DLIB}\plugins\platformthemes; Flags: recursesubdirs; Components: core

Source: {#QT}\plugins\renderplugins\scene2d.dll; DestDir: {#DLIB}\plugins\renderplugins; Flags: recursesubdirs; Components: core

Source: {#QT}\resources\*; DestDir: {#DLIB}\resources; Flags: recursesubdirs; Components: core
Source: {#QT}\translations\*; DestDir: {#DLIB}\translations; Flags: recursesubdirs; Components: core


Source: {#BOOST}\lib\boost_locale-vc142-mt-x64-1_70.dll; DestDir: {#DBIN}; Flags: skipifsourcedoesntexist; Components: core

Source: {#OSI}\bin\open_simulation_interface.dll; DestDir: {#DBIN}; Flags: skipifsourcedoesntexist; Components: core  
 
Source: {#PROTOBUF}\bin\libprotobuf.dll; DestDir: {#DBIN}; Flags: skipifsourcedoesntexist; Components: core
Source: {#PROTOBUF}\bin\libprotobuf-lite.dll; DestDir: {#DBIN}; Flags: skipifsourcedoesntexist; Components: core
Source: {#PROTOBUF}\bin\libprotoc.dll; DestDir: {#DBIN}; Flags: skipifsourcedoesntexist; Components: core         

Source: {#ICU}\bin64\icuuc58.dll; DestDir: {#DBIN}; Flags: skipifsourcedoesntexist; Components: core   
Source: {#ICU}\bin64\icuin58.dll; DestDir: {#DBIN}; Flags: skipifsourcedoesntexist; Components: core      
Source: {#ICU}\bin64\icudt58.dll; DestDir: {#DBIN}; Flags: skipifsourcedoesntexist; Components: core       
Source: {#ICONFILE}; DestDir: {#DBIN}; Flags: skipifsourcedoesntexist; Components: core
                                                                

Source: {#OP_SOURCE}\Documentation; DestDir: {app}\Documentation; Excludes: .svn\*,*.tex,*.aux,*.log,*.pl,WARNINGS; Flags: recursesubdirs; Components: help

                     
Source: {#OP_SOURCE}\scripts\windowsEnv.bat; DestDir: {app}\scripts; Components: core
[Registry]

; for user installation
;Root: HKCU; Subkey: Environment; ValueType: string; ValueName: PATH; ValueData: "{code:getShortAppDir|{app}}\bin;{code:getShortAppDir|{app}}"; Flags: uninsdeletekeyifempty uninsdeletevalue; Check: InstallForUser()

;Root: HKCU; Subkey: Environment; ValueType: string; ValueName: OPENPASS_HOME; ValueData: {code:getShortAppDir|{app}}; Flags: uninsdeletekeyifempty uninsdeletevalue; Check: InstallForUser()

; end for user installation
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: OPENPASS_HOME; ValueData: {code:getShortAppDir|{app}}; Flags: uninsdeletekeyifempty uninsdeletevalue

Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\OpenPASS.exe; ValueType: string; ValueData: {code:getShortAppDir|{app}}\OpenPASS.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\OpenPassMaster.exe; ValueType: string; ValueData: {code:getShortAppDir|{app}}\OpenPassMaster.exe; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\OpenPassSlave.exe; ValueType: string; ValueData: {code:getShortAppDir|{app}}\OpenPassSlave.exe; Flags: uninsdeletekeyifempty uninsdeletevalue

Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\OpenPASS.exe; ValueType: string; ValueName: Path; ValueData: "{code:getShortAppDir|{app}}\bin;{app}};{app}\lib;"; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\OpenPassMaster.exe; ValueType: string; ValueName: Path; ValueData: "{code:getShortAppDir|{app}}\bin;{app}};{app}\lib;"; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\OpenPassSlave.exe; ValueType: string; ValueName: Path; ValueData: "{code:getShortAppDir|{app}}\bin;{app}};{app}\lib;"; Flags: uninsdeletekeyifempty uninsdeletevalue


[INI]

Filename: {app}\qt.conf; Section: Paths
Filename: {app}\qt.conf; Section: Paths; Key: Prefix; String: {code:GetWithForwardSlashes|{code:GetShortName|{#DLIB}}}
Filename: {app}\qt.conf; Section: Paths; Key: Documentation; String: doc
Filename: {app}\qt.conf; Section: Paths; Key: Headers; String: include
Filename: {app}\qt.conf; Section: Paths; Key: Libraries; String: lib
Filename: {app}\qt.conf; Section: Paths; Key: Binaries; String: bin
Filename: {app}\qt.conf; Section: Paths; Key: Plugins; String: plugins
Filename: {app}\qt.conf; Section: Paths; Key: Translations; String: translations
Filename: {app}\qt.conf; Section: Paths; Key: Examples; String: examples
Filename: {app}\qt.conf; Section: Paths; Key: Demos; String: demos


[UninstallDelete]
Type: files; Name: {app}\qt.conf

[Tasks]
Name: desktopicon; Description: Icons on &Desktop; GroupDescription: Desctop Icons:
Name: startupcion; Description: Icons into &Startup; GroupDescription: Startup Icons:; Flags: unchecked


[Dirs]
Name: "{app}"; 
Name: "{app}\configs"; Permissions: everyone-full


[Icons]
Name: {group}\OpenPASS; Filename: {app}\OpenPASS.exe; Comment: Start OpenPASS; IconFilename: {#DICONFILE}; Flags: createonlyiffileexists
Name: {group}\OpenPASS Shell; Filename: {cmd}; Parameters: "/K cd ""%OPENPASS_HOME%\configs\"" && ""%OPENPASS_HOME%\scripts\windowsEnv.bat"""; WorkingDir: {app}\configs; Comment: Command Prompt with OpenPASS Environment; IconFilename: {#DICONFILE}

Name: {group}\Documentation\Tutorial; Filename: {#DDOC}\Tutorial_openPass_GUI.pdf; Flags: createonlyiffileexists
Name: {group}\Documentation\Glossary; Filename: {#DDOC}\Glossary.pdf; Flags: createonlyiffileexists
Name: {group}\Documentation\PCM; Filename: {#DDOC}\openPASS_Setup_PCM_Win7x64.pdf; Flags: createonlyiffileexists
Name: {group}\Documentation\OSI; Filename: {#DDOC}\OSI World Setup Guide.pdf; Flags: createonlyiffileexists
Name: {group}\Uninstall OpenPASS; Filename: {uninstallexe}

Name: {commondesktop}\OpenPASS; Filename: {app}\bin\OpenPASS.exe; Comment: Start OpenPASS; IconFilename: {#ICONFILE}; Flags: createonlyiffileexists; Tasks: desktopicon

[Run]
Filename: {app}\bin\VC_redist.x64.exe; Parameters: /Q; Description: Install VisualStudio 2019 x64 Runtime; Flags: postinstall

[Code]
(*
program Setup;


var

  CheckInstallRevitPlugin,CheckInstallForAll,CheckInstallRemoteDaemon,CheckInstallRemoteDaemonForAll: TCheckBox;
  UNCPathName: TEdit;
  Page: TWizardPage;



procedure FormButtonOnClick(Sender: TObject);
var
  Form: TSetupForm;
  OKButton, CancelButton: TButton;
begin
  Form := CreateCustomForm();
  try
    Form.ClientWidth := ScaleX(256);
    Form.ClientHeight := ScaleY(256);
    Form.Caption := 'TSetupForm';
    //Form.CenterInsideControl(WizardForm, False);

    OKButton := TButton.Create(Form);
    OKButton.Parent := Form;
    OKButton.Width := ScaleX(75);
    OKButton.Height := ScaleY(23);
    OKButton.Left := Form.ClientWidth - ScaleX(75 + 6 + 75 + 10);
    OKButton.Top := Form.ClientHeight - ScaleY(23 + 10);
    OKButton.Caption := 'OK';
    OKButton.ModalResult := mrOk;

    CancelButton := TButton.Create(Form);
    CancelButton.Parent := Form;
    CancelButton.Width := ScaleX(75);
    CancelButton.Height := ScaleY(23);
    CancelButton.Left := Form.ClientWidth - ScaleX(75 + 10);
    CancelButton.Top := Form.ClientHeight - ScaleY(23 + 10);
    CancelButton.Caption := 'Cancel';
    CancelButton.ModalResult := mrCancel;
    CancelButton.Cancel := True;

    Form.ActiveControl := OKButton;


    if Form.ShowModal() = mrOk then
      MsgBox('You clicked OK.', mbInformation, MB_OK);
  finally
    Form.Free();
  end;
end;



procedure CreateTheWizardPages;
   var
      appname : String;
      Lbl11 : TLabel;
begin
  { TButton and others }

  appname := 'OpenPASS';
  Page := CreateCustomPage(wpWelcome, 'OpenPASS Installation','Open Platform for Assessment of Safety Systems');


  Lbl11 := TLabel.Create(Page);
  Lbl11.Caption :='This is an EPL version of OpenPASS';
  Lbl11.AutoSize := True;

  Lbl11.Parent := Page.Surface;

    if IsAdminInstallMode then
    begin
  CheckInstallForAll := TCheckBox.Create(Page);
  CheckInstallForAll.Top := Lbl11.Top + Lbl11.Height + ScaleY(8);
  CheckInstallForAll.Width := Page.SurfaceWidth;
  CheckInstallForAll.Height := ScaleY(17);
  CheckInstallForAll.Caption := 'Install ' + appname + ' for all users?';
  CheckInstallForAll.Checked := True;
  CheckInstallForAll.Parent := Page.Surface;
              

    end;

end;
function NextButtonClick(CurPageID: Integer): Boolean;
begin
  if CurPageID = 2 then
  begin
     UNCPathName.Text := ExpandConstant('{app}');
  end;
  Result := true;


end;

procedure AboutButtonOnClick(Sender: TObject);
begin
  MsgBox('This Wizard allows to install and configure OpenPASS', mbInformation, mb_Ok);
end;



procedure URLLabelOnClick(Sender: TObject);
var
  ErrorCode: Integer;
begin
  ShellExec('open', 'https://projects.eclipse.org/projects/technology.simopenpass', '', '', SW_SHOWNORMAL, ewNoWait, ErrorCode);
end;





procedure InitializeWizard();
var
  AboutButton, CancelButton: TButton;
  URLLabel: TNewStaticText;
  BackgroundBitmapImage: TBitmapImage;
  BackgroundBitmapText: TNewStaticText;
begin
  { Custom wizard pages }


  CreateTheWizardPages;

  { Other custom controls }

  CancelButton := WizardForm.CancelButton;

  AboutButton := TButton.Create(WizardForm);
  AboutButton.Left := WizardForm.ClientWidth - CancelButton.Left - CancelButton.Width;
  AboutButton.Top := CancelButton.Top;
  AboutButton.Width := CancelButton.Width;
  AboutButton.Height := CancelButton.Height;
  AboutButton.Caption := '&About...';
  AboutButton.OnClick := @AboutButtonOnClick;
  AboutButton.Parent := WizardForm;

  URLLabel := TNewStaticText.Create(WizardForm);
  URLLabel.Caption := 'eclipse page';
  URLLabel.Cursor := crHand;
  URLLabel.OnClick := @URLLabelOnClick;
  URLLabel.Parent := WizardForm;
  { Alter Font *after* setting Parent so the correct defaults are inherited first }
  URLLabel.Font.Style := URLLabel.Font.Style + [fsUnderline];
  URLLabel.Font.Color := clBlue;
  URLLabel.Top := AboutButton.Top + AboutButton.Height - URLLabel.Height - 2;
  URLLabel.Left := AboutButton.Left + AboutButton.Width + ScaleX(20);

  BackgroundBitmapImage := TBitmapImage.Create(MainForm);
  BackgroundBitmapImage.Left := 50;
  BackgroundBitmapImage.Top := 100;
  BackgroundBitmapImage.AutoSize := True;
  BackgroundBitmapImage.Bitmap := WizardForm.WizardBitmapImage.Bitmap;
  BackgroundBitmapImage.Parent := MainForm;

  BackgroundBitmapText := TNewStaticText.Create(MainForm);
  BackgroundBitmapText.Left := BackgroundBitmapImage.Left;
  BackgroundBitmapText.Top := BackgroundBitmapImage.Top + BackgroundBitmapImage.Height + ScaleY(8);
  BackgroundBitmapText.Caption := 'TBitmapImage';
  BackgroundBitmapText.Parent := MainForm;


end;

      
procedure CurStepChanged(CurStep: TSetupStep);
var
   filecontent, filename: String;
begin
if CurStep = ssPostInstall then begin
// do whatever is needed after installation
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
if CurUninstallStep = usPostUninstall then begin
  // uninstall script
  
  end;
end;

function InstallForAll(): Boolean;
begin

  Result := False;
  if IsAdminInstallMode then
  begin
     Result := CheckInstallForAll.Checked;
  end;
end;     


function InstallForUser(): Boolean;
begin
     Result := NOT InstallForAll();
end;


       
*)

function getShortAppDir(appName: String ): String;
begin
     //if UNCPathName.Text = '' then
     begin
        Result := appName;
     end
     //else begin
     //   Result := GetShortName(UNCPathName.Text);
     //end;
end;

function GetWithForwardSlashes(Param: String): String;
var
  mytmpstr : String;
begin
  mytmpstr := ExpandConstant(Param);
  StringChangeEx(mytmpstr, '\', '/', True);
  Result := mytmpstr;
end;

 (*

begin
end.
   *)