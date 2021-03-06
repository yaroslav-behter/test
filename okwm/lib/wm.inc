<?php

// 10.09.2001
// (win1251 encoded)
//
//----------   WM HTTPS-interface example
//----------   ������ ������������� WM HTTPS-�����������
//
// Webmoney Transfer (c), Shaposhnikov Max    (maxicus@hotmail.com)

//          ******************************************************
//          ***   ������ ���������� ������� htts-�����������   ***
//          ******************************************************

include("wmconst.inc");
//require_once("${HTTP_SERVER_VARS['DOCUMENT_ROOT']}/lib/wmconst.inc");
require_once("${HTTP_SERVER_VARS['DOCUMENT_ROOT']}/lib/wmparser.inc");

/////////////////////////////////////////////////////////////////
// _DateID - ���������� ���������� 14-�������� ��������.
//           ��� ����������� ����������� ��������� ������� ������
//
// ���������� :
//   ������ - ���������� ��������

function _DateID()
{
  $tm = localtime(time(), 1);

  return sprintf( "%04d%02d%02d%02d%02d%02d", $tm["tm_year"]+1900, $tm["tm_mon"]+1,
                  $tm["tm_mday"], $tm["tm_hour"], $tm["tm_min"], $tm["tm_sec"]
                );
}



/////////////////////////////////////////////////////////////////
// _GetSign - ������������ ������ �������.
//            ��� ����������� ����������� ��������� ������� ������
//
// ��������� :
//   $inStr  - ������ ��� �������
// ���������� :
//   ������ - �������
//
//! ��� ������ ������� � ��� �� ����������, ��� � ���������� ������ (�� �������
//! � ������ ������������ ������� �����-������), ������ ������������ �����
//! WMSigner � WMSigner.ini

function _GetSign($inStr)
{
  global $HTTP_SERVER_VARS;
  //$root = $HTTP_SERVER_VARS['DOCUMENT_ROOT'];
  $root = "c:/var/spool/okwm/wm";
  $tmp  = tempnam ("/tmp","wm");
  $PlanStr = "$inStr\004\r\n";
  $fp = fopen ($tmp, "w");
  fwrite($fp,$PlanStr);
  fclose($fp);
  $res = exec ("$root/WMSigner.exe <$tmp", $a);
  unlink($tmp);
  return $res;
}



/////////////////////////////////////////////////////////////////
// _HttpsReq - ���������� https-������� � ����������������� ������ webmoney
//             ��� ����������� ����������� ��������� ������� ������
//
// ��������� :
//   $addr  - ����� ������� (������� � ����� �������)
//            ��. "/cgi-bin/myscript.cgi?PRM1=VAL1"
// ���������� :
//   ��������� ������ https-������� (��� ����������)

function _HttpsReq($addr, $post="")
{
  global $wmCAcert;
  $ch = curl_init("https://w3s.webmoney.ru".$addr);
  curl_setopt($ch, CURLOPT_HEADER, 0);
  // CURLOPT_RETURNTRANSFER ����� �������� �����������, �� ���������
  curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
  //curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);
  //curl_setopt($ch, CURLOPT_SSLVERSION, 3);
  curl_setopt($ch, CURLOPT_CAINFO, $wmCAcert);
  if ($post !== "")
  {
     curl_setopt ($ch, CURLOPT_POST, 1);
     curl_setopt ($ch, CURLOPT_POSTFIELDS, $post);
  }

  //$tmp = tempnam ("/tmp","wm");
  //$fp = fopen ($tmp,"w");
  //curl_setopt ($ch, CURLOPT_FILE, $fp);
  $result = curl_exec ($ch);
  /*if (getenv("REMOTE_ADDR")=="82.207.125.214")  {      echo curl_error($ch);
      echo " res=".$result;
  }*/

  curl_close($ch);
  //fclose($fp);

  //$fp = fopen ($tmp,"r");
  //$result = fgets($fp,1024);
  //fclose($fp);
  //unlink($tmp);
  return $result;
}



/////////////////////////////////////////////////////////////////
// InvCreate - ������� �����
//
// ��������� :
//   $wmid   - WM������������� ���������� �����
//   $summ   - ����� ����� (. - �����������)
//   $inv_id - � ����� � ��������
//   $dsc    - �������� ������� (win1251 encoding)
//   $adr    - ����� �������� (win1251 encoding)
// ���������� :
//   2-���������� ������ ($wminv_id, $err), ���
//     $wminv_id - ����� ���������� ����� (���������� ����� WM-�����)
//     $err      - ����� ������
//
// ��� �������� ��������� $wminv_id �������� ������������� ����� ����������
// ����� (��� ���������� ����� �����, ���������� ����� ���� ������ WM).
// � ������ ������ $wminv_id �������� ������ ������, � $err - ����� ������,
// ��������� � �����.
//
//! ��� ������ ������� � ��� �� ����������, ��� � ���������� ������ (�� �������
//! � ������ ������������ ������� �����-������), ������ ������������ �����
//! WMSigner � WMSigner.ini

function InvCreate($wmid, $summ, $inv_id, $dsc, $adr, $purse, $protect=0)
{
  $summ = sprintf("%d", $summ*100)/100;   // ������������ �����

  global $wmconst__shop_wmzpurse;
  global $wmconst__shop_wmepurse;
  global $wmconst__shop_wmrpurse;
  global $wmconst__shop_wmupurse;
  global $wmconst__shop_wmid;
  global $wmconst__invc_active;

  $OrderID       = "$inv_id";                   // � �����
  // ������� � ��������, ���� ����� ����������� ������
  if ($purse=="E") { $PurseOfStores = "$wmconst__shop_wmepurse"; }
  if ($purse=="Z") { $PurseOfStores = "$wmconst__shop_wmzpurse"; }
  if ($purse=="R") { $PurseOfStores = "$wmconst__shop_wmrpurse"; }
  if ($purse=="U") { $PurseOfStores = "$wmconst__shop_wmupurse"; }
  $LoginOfStores = "$wmconst__shop_wmid";       // WM id ��������
  $LoginOfCust   = "$wmid";                     // WM id ����������
  $InvAddress    = "$adr";                      // ����� ��������
  $Amount        = "$summ";                     // �����
  $Desc          = "$dsc";                      // �������� �������
  $Period        = "$protect";                  // ���� ���������
  $Experation    = "$wmconst__invc_active";     // ����� ���������������� �����
  $RequestN      = _DateID();                   // ��������� ������������ ����� �������

  // ������������ ������ ��� �������
  $PlanStr = "$OrderID$LoginOfCust$PurseOfStores$Amount$Desc$InvAddress$Period$Experation$RequestN";
  // ������������ �������
  $SignStr = _GetSign($PlanStr);
  // https-������
  $W3sUrl="/asp/Invoice.asp?SL=$LoginOfStores&SP=$PurseOfStores&CL=$LoginOfCust&IN=$OrderID&D=".
          urlencode($Desc)."&AD=".urlencode($InvAddress).
          "&A=$Amount&E=$Experation&P=$Period&RN=$RequestN&SS=$SignStr";

  $httpsRes = _HttpsReq($W3sUrl);

  // ������ ����������� �� ����������������� ����� ����������
  $err      = "������ ����� � ���������������� ������� WebMoney";
  $wminv_id = "";

  if (substr($httpsRes,0,6) == "Error:")
  {
    $errCode = sprintf("%d", substr($httpsRes,6));

    $err = "����������� ������";
    if ($errCode == "-9")
    { $err = "�������� �������"; }
    elseif ($errCode == "-8")
    { $err = "R==uestN ����� �����������"; }
    elseif ($errCode == "-7")
    { $err = "������� ������� ����� ��������"; }
    elseif ($errCode == "-6")
    { $err = "������� ������� ��������"; }
    elseif ($errCode == "-5")
    { $err = "������� ������ �����"; }
    elseif ($errCode == "-4")
    { $err = "������� ����� WM-������������� web-�������"; }
    elseif ($errCode == "-3")
    { $err = "������� ����� WM-������� web-�������"; }
    elseif ($errCode == "-2")
    { $err = "������� ����� WM-������������� ����������"; }
    elseif ($errCode == "-1")
    { $err = "������� ����� ����� �����"; }
    elseif ($errCode == "5")
    { $err = "Web-������� � ��������� ��������������� �� ����������"; }
    elseif ($errCode == "6")
    { $err = "���������� � ��������� ��������������� �� ����������"; }
    elseif ($errCode == "7")
    { $err = "���������� WM-�������� web-������� �� ����������"; }
    elseif ($errCode == "13")
    { $err = "������� ��������� �����"; }
    elseif ($errCode == "23")
    { $err = "������ �������"; }
    elseif ($errCode == "102")
    { $err = "�������� R==uestN ������ ��� ����� ����������� ��������� R==uestN"; }
  }

  if (substr($httpsRes,0,8) == "WMInvId:")
  {
    $wminv_id = sprintf("%d", substr($httpsRes,8));
    $err      = "";
  }

  return array($wminv_id, $err);
}



/////////////////////////////////////////////////////////////////
// InvCheck - �������� ��������� �����
//
// ��������� :
//   $inv_id   - ���������� � ����� �������� ��� ��������
//   $wmid     - WM������������� ���������� �����
//   $wminv_id - ���������� � WM ����� ��� �������� (����� ��������� ��� $inv_id)
//   $purse    - ��� ������� � ��������, ���� ������ ���� ����������� ������
// ���������� :
//   ��������� �����
//     -2 - ������ �������� ���������
//     -1 - ������
//      0 - ���� ������
//      1 - �������, �� ������ �� �������� �� ������� ������� ��������� ������
//      2 - �������
//
// ���� ������� $wminv_id, �� �������� $inv_id ������������.
//
//! ��� ������ ������� � ��� �� ����������, ��� � ���������� ������ (�� �������
//! � ������ ������������ ������� �����-������), ������ ������������ �����
//! WMSigner � WMSigner.ini

function InvCheck($inv_id, $wmid, $wminv_id, $purse)
{
  $inv_id   = sprintf("%d", $inv_id);
  $wminv_id = sprintf("%d", $wminv_id);

  global $wmconst__shop_wmid;
  global $wmconst__shop_wmzpurse;
  global $wmconst__shop_wmepurse;
  global $wmconst__shop_wmrpurse;
  global $wmconst__shop_wmupurse;

  $LoginOfStores = "$wmconst__shop_wmid";      // WM id ��������
  $WMInvId       = "$wminv_id";                // WM� �����
  // ������� � ��������, ������ ����� ����������� ������
  if ($purse=="E") { $PurseOfStores = "$wmconst__shop_wmepurse"; }
  if ($purse=="Z") { $PurseOfStores = "$wmconst__shop_wmzpurse"; }
  if ($purse=="R") { $PurseOfStores = "$wmconst__shop_wmrpurse"; }
  if ($purse=="U") { $PurseOfStores = "$wmconst__shop_wmupurse"; }
  $LoginOfCust   = "$wmid";                    // WM id ����������
  $OrderID       = "$inv_id";                  // � ����� � ��������
  $RequestN      = _DateID();                  // ��������� ������������ ����� �������

  // ������������ ������ ��� �������
  $PlanStr = "$OrderID$WMInvId$LoginOfCust$PurseOfStores$RequestN";
  // ������������ �������
  $SignStr = _GetSign($PlanStr);


  // https-������
  $W3sUrl="/asp/GetInvState.asp?SL=$LoginOfStores&WN=$WMInvId&SP=$PurseOfStores&CL=$LoginOfCust&IN=$OrderID&RN=$RequestN&SS=$SignStr";

  $httpsRes = _HttpsReq($W3sUrl);


  // ������ ����������� �� ����������������� ����� ����������
  $state = -2;
  // State:0 - ���� ��� �� ������� �����������;
  if (substr($httpsRes,0,7) == "State:0")  { $state =  0; }
  // State:1 - ���� �� ��� ������� ����������� � ���������� ��������� �� ������ �����;
  elseif (substr($httpsRes,0,7) == "State:1")  { $state = -1; }
  // Ok:1 - ���� ������� - ������ ��� ����������� �� ������� ��������;
  elseif (substr($httpsRes,0,4) == "Ok:1")     { $state =  2; }
  // Ok:2 - ���� �������, �� �� �������� � ���������� - ������ ��� �� � ��������, ��� ���������� ���������� ������ ��� ��������� � ������� ����� ��������� � WebMoney Keeper ��������;
  elseif (substr($httpsRes,0,4) == "Ok:2")     { $state =  1; }
  // State:2 - ���� ��� �������, �� ������ ���������� ����� ����������� � ����� � ���, ��� ���� ����� 8-�� ������� �������� ���� ��������� ��� ����� ���� ���������, � ��� ��� � �� ��� ������ - ����� ������������ �� ������� ����� � �������� ���;
  elseif (substr($httpsRes,0,7) == "State:2")  { $state = -1; }
  // Error: Invoice Not Found
  elseif (substr($httpsRes,0,6) == "Error:Invoice Not Found")   { $state = -2; }

  return $state;
}



/////////////////////////////////////////////////////////////////
// TransCreate - �������� �������
//
// ��������� :
//   $wmpurse - �������, �� ������� ������������� �������� �������
//   $summ    - ����� ��������
//   $trn_id  - � ���������� (������ ���� ���������� ��� ��������)
//   $dsc     - ���������� � �������� (win1251)
//   $purse   - ��� ��������, � �������� ����� ���������� �������
// ���������� :
//   2-���������� ������ ($wmtrn_id, $err), ���
//     $wmtrn_id - ����� ���������� �������� (���������� ����� WM-��������)
//     $err      - ����� ������
//
// ��� �������� ��������� $wmtrn_id �������� ������������� ����� ����������
// �������� (��� ���������� �����, ���������� ����� ���� ��������� WM).
// � ������ ������ $wmtrn_id �������� ������ ������, � $err - ����� ������,
// ��������� � �����.
//
//! ��� ������ ������� � ��� �� ����������, ��� � ���������� ������ (�� �������
//! � ������ ������������ ������� �����-������), ������ ������������ �����
//! WMSigner � WMSigner.ini

function TransCreate($wmpurse, $summ, $trn_id, $dsc, $purse)
{
  $RequestN = _DateID();    // ��������� ������������ ����� �������

  global $wmconst__shop_wmzpurse;
  global $wmconst__shop_wmepurse;
  global $wmconst__shop_wmrpurse;
  global $wmconst__shop_wmupurse;

  // ������� � ��������, ������ ����� ����������� ������
  if ($purse=="E") { $wmconst__shop_wmpurse = "$wmconst__shop_wmepurse"; }
  if ($purse=="Z") { $wmconst__shop_wmpurse = "$wmconst__shop_wmzpurse"; }
  if ($purse=="R") { $wmconst__shop_wmpurse = "$wmconst__shop_wmrpurse"; }
  if ($purse=="U") { $wmconst__shop_wmpurse = "$wmconst__shop_wmupurse"; }

  // ������������ ������ ��� �������
  $PlanStr = "$trn_id$wmconst__shop_wmpurse$wmpurse$summ$dsc".
                "0"."$RequestN";
  // ������������ �������
  $SignStr = _GetSign($PlanStr);


  // https-������
  $W3sUrl = "/asp/Trans.asp?TID=$trn_id&P=$wmconst__shop_wmpurse&C=$wmpurse&IN=0&A=$summ&D=".
            urlencode($dsc)."&RN=$RequestN&SS=$SignStr";

  $httpsRes = _HttpsReq($W3sUrl);
  //$tmp = tempnam ("/tmp","wm");
  //$fp = fopen ($tmp,"w");
  //fwrite($fp, $httpsRes);
  //fclose($fp);


  // ������ ����������� �� ����������������� ����� ����������
  $err      = "������ ����� � ���������������� ������� WebMoney";
  $wmtrn_id = "";

  if (substr($httpsRes,0,6) == "Error:")
  {
    $errCode = sprintf("%d", substr($httpsRes,6));

    $err = "����������� ������";
    if ($errCode == "-6")
    { $err = "�������� �������"; }
    elseif ($errCode == "-5")
    { $err = "R==uestN ����� �����������"; }
    elseif ($errCode == "-4")
    { $err = "������� ������ �����"; }
    elseif ($errCode == "-3")
    { $err = "������� ����� �������-��������"; }
    elseif ($errCode == "-2")
    { $err = "������� ����� �������-��������"; }
    elseif ($errCode == "-1")
    { $err = "������� ����� ����� ��������"; }
    elseif ($errCode == "5")
    { $err = "�������� � ��������� ��������������� �� ����������"; }
    elseif ($errCode == "6")
    { $err = "���������� � ��������� ��������������� �� ����������"; }
    elseif ($errCode == "7")
    { $err = "���������� �������� �������� �� ����������"; }
    elseif ($errCode == "13")
    { $err = "������� ��������� �����"; }
    elseif ($errCode == "17")
    { $err = "������������ ����� � ������-���������"; }
    elseif ($errCode == "23")
    { $err = "������ �������"; }
    elseif ($errCode == "102")
    { $err = "�������� R==uestN ������ ��� ����� ����������� ��������� R==uestN"; }
    elseif ($errCode == "103")
    { $err = "�������� � ����� ������� (TID) ��� �����������"; }
    elseif ($errCode == "110")
    { $err = "��������� ����������. ".
             "���� ��������� �������� ������ ����������������� ��������. ".
             "��� ����������� ��������� �� ���������� ����� ".
             "� WMID 941977853154 (����������� ���������)";
    }
  }

  if (substr($httpsRes,0,9) == "WMTranId:")
  {
    $wmtrn_id = sprintf("%d", substr($httpsRes,9));
    $err      = "";
  }

  return array($wmtrn_id, $err);
}



/////////////////////////////////////////////////////////////////
// TransCreateProt - �������� �������
//
// ��������� :
//   $wmpurse - �������, �� ������� ������������� �������� �������
//   $summ    - ����� ��������
//   $trn_id  - � ���������� (������ ���� ���������� ��� ��������)
//   $dsc     - ���������� � �������� (win1251)
//   $pr_code - ��� ��������� (win1251)
// ���������� :
//   2-���������� ������ ($wmtrn_id, $err), ���
//     $wmtrn_id - ����� ���������� �������� (���������� ����� WM-��������)
//     $err      - ����� ������
//
// ��� �������� ��������� $wmtrn_id �������� ������������� ����� ����������
// �������� (��� ���������� �����, ���������� ����� ���� ��������� WM).
// � ������ ������ $wmtrn_id �������� ������ ������, � $err - ����� ������,
// ��������� � �����.
//
//! ��� ������ ������� � ��� �� ����������, ��� � ���������� ������ (�� �������
//! � ������ ������������ ������� �����-������), ������ ������������ �����
//! WMSigner � WMSigner.ini

function TransCreateProt($wmpurse, $summ, $trn_id, $dsc, $pr_code)
{
  $RequestN      = _DateID();    // ��������� ������������ ����� �������

  global $wmconst__trans_prtime;
  global $wmconst__shop_wmpurse;

  // ������������ ������ ��� �������
  $PlanStr = "$trn_id$wmconst__shop_wmpurse$wmpurse$summ$dsc".
             "0"."$pr_code"."$wmconst__trans_prtime"."$RequestN";
  // ������������ �������
  $SignStr = _GetSign($PlanStr);


  // https-������
  $W3sUrl = "/asp/TransDelayed.asp?TID=$trn_id&P=$wmconst__shop_wmpurse&C=$wmpurse&IN=0&A=$summ&D=".
            urlencode($dsc)."&PC=".urlencode($pr_code)."&PL=$wmconst__trans_prtime".
            "&RN=$RequestN&SS=$SignStr";

  $httpsRes = _HttpsReq($W3sUrl);


  // ������ ����������� �� ����������������� ����� ����������
  $err      = "������ ����� � ���������������� ������� WebMoney";
  $wmtrn_id = "";

  if (substr($httpsRes,0,6) == "Error:")
  {
    $errCode = sprintf("%d", substr($httpsRes,6));

    $err = "����������� ������";
    if ($errCode == "-10")
    { $err = "�������� ������ ���������"; }
    elseif ($errCode == "-9")
    { $err = "�������� ��� ���������"; }
    elseif ($errCode == "-6")
    { $err = "�������� �������"; }
    elseif ($errCode == "-5")
    { $err = "R==uestN ����� �����������"; }
    elseif ($errCode == "-4")
    { $err = "������� ������ �����"; }
    elseif ($errCode == "-3")
    { $err = "������� ����� �������-��������"; }
    elseif ($errCode == "-2")
    { $err = "������� ����� �������-��������"; }
    elseif ($errCode == "-1")
    { $err = "������� ����� ����� ��������"; }
    elseif ($errCode == "5")
    { $err = "����������� ������� � ��������� ��������������� �� ����������"; }
    elseif ($errCode == "6")
    { $err = "���������� � ��������� ��������������� �� ����������"; }
    elseif ($errCode == "7")
    { $err = "�������� ����������� ������� �� ����������"; }
    elseif ($errCode == "13")
    { $err = "������� ��������� �����"; }
    elseif ($errCode == "17")
    { $err = "������������ ����� � ������-���������"; }
    elseif ($errCode == "23")
    { $err = "������ �������"; }
    elseif ($errCode == "102")
    { $err = "�������� RequestN ������ ��� ����� ����������� ��������� RequestN"; }
    elseif ($errCode == "103")
    { $err = "�������� � ����� ������� (TID) ��� �����������"; }
    elseif ($errCode == "110")
    { $err = "��������� ����������. ".
             "���� ��������� �������� ������ ����������������� ��������. ".
             "��� ����������� ��������� �� ���������� ����� ".
             "� WMID 941977853154 (����������� ���������)";
    }
  }

  if (substr($httpsRes,0,9) == "WMTranId:")
  {
    $wmtrn_id = sprintf("%d", substr($httpsRes,9));;
    $err      = "";
  }

  return array($wmtrn_id, $err);
}



/////////////////////////////////////////////////////////////////
// TransCheck - �������� ���������� ���������� ��������� ��������
//
// ��������� :
//   $trn_id  - � ���������� �������� (������ ���� ���������� ��� ��������)
// ���������� :
//   0 - ������ ��� ��������
//   1 - �� ����������
//   2 - ����������
//
//! ��� ������ ������� � ��� �� ����������, ��� � ���������� ������ (�� �������
//! � ������ ������������ ������� �����-������), ������ ������������ �����
//! WMSigner � WMSigner.ini

function TransCheck($trn_id)
{
  $RequestN = _DateID();         // ��������� ������������ ����� �������

  global $wmconst__shop_wmpurse;

  // ������������ ������ ��� �������
  $PlanStr = "0$trn_id$wmconst__shop_wmpurse$RequestN";
  // ������������ �������
  $SignStr = _GetSign($PlanStr);

  // https-������
  $W3sUrl = "/asp/GetTran.asp?CTID=$trn_id&WTID=0&P=$wmconst__shop_wmpurse&RN=$RequestN&SS=$SignStr";

  $httpsRes = _HttpsReq($W3sUrl);


  // ������ ����������� �� ����������������� ����� ����������
  if (substr($httpsRes,0,6) == "Error:")
  { return 0; }
  elseif (substr($httpsRes,0,10) == "TranState:")
  { if (substr($httpsRes,10) > 0)
    { return 2; }
    return 1;
  }

  return 0;
}



/////////////////////////////////////////////////////////////////
// SendMsg - ������� ���������
//
// ��������� :
//   $wmid - WM������������� ���������� ���������
//   $msg - ����� ���������
// ���������� :
//   ����� ������ ��� ������ ������ � ������ ������
//
//! ��� ������ ������� � ��� �� ����������, ��� � ���������� ������ (�� �������
//! � ������ ������������ ������� �����-������), ������ ������������ �����
//! WMSigner � WMSigner.ini

function SendMsg($wmid, $msg)
{
  $RequestN = _DateID();         // ��������� ������������ ����� �������

  global $wmconst__shop_wmid;

  // ������������ ������ ��� �������
  $PlanStr = "$wmconst__shop_wmid$wmid$msg$RequestN";
  // ������������ �������
  $SignStr = _GetSign($PlanStr);

  // https-������
  $W3sUrl = "/asp/SendMsg.asp?S=$wmconst__shop_wmid&R=$wmid&M=".urlencode($msg)."&RN=$RequestN&SS=$SignStr";
  $httpsRes = _HttpsReq($W3sUrl);


  // ������ ����������� �� ����������������� ����� ����������
  if (substr($httpsRes,0,6) == "Error:")
  {
    $errCode = sprintf("%d", substr($httpsRes,6));

    $err = "����������� ������";
    if ($errCode == "-7")
    { $err = "������� �������������� �������, �������� ��� �� ������"; }
    elseif ($errCode == "-6")
    { $err = "���������� ������ �������� ���������"; }
    elseif ($errCode == "-5")
    { $err = "����������� ��� ������� ������ �������"; }
    elseif ($errCode == "-4")
    { $err = "R==uestN ����� �����������"; }
    elseif ($errCode == "-3")
    { $err = "����������� ��� ������� ����� ����� ���������"; }
    elseif ($errCode == "-2")
    { $err = "����������� ��� ������� ����� wm-������������� ����������"; }
    elseif ($errCode == "-1")
    { $err = "����������� ��� ������� ����� wm-������������� �����������"; }
    elseif ($errCode == "5")
    { $err = "����������� ��������� � ��������� ��������������� �� ����������"; }
    elseif ($errCode == "6")
    { $err = "���������� ��������� � ��������� ��������������� �� ����������"; }
    elseif ($errCode == "14")
    { $err = "������ �������"; }
    elseif ($errCode == "102")
    { $err = "�������� R==uestN ������ ��� ����� ����������� ��������� R==uestN"; }
    elseif ($errCode == "103")
    { $err = "�������� � ����� ������� (TID) ��� �����������"; }
    elseif ($errCode == "110")
    { $err = "��������� ����������. ".
             "���� ��������� �������� ������ ����������������� ��������. ".
             "��� ����������� ��������� �� ���������� ����� ".
             "� WMID 941977853154 (����������� ���������)";
    }

    return $err;
  }

  return "";
}



/////////////////////////////////////////////////////////////////
// TestAutority - �������� ���������� ������� ����� ����������� ��������������
//
// ��������� :
//   $cliWMID    - WM-������������� �������, �������� ���������� �����������������
//   $cliPlanStr - ��������, ����������� ��������
//   $cliSignStr - ������� ������� ��� ��������������
// ���������� :
//   2-���������� ������ ($status, $err), ���
//     $status -1 ������ ��� ��������
//              0 ������ �� ������ ��������������
//              1 ������ ������ ��������������
//     $err    ����� ������ (��� status = -1)
//
//! ��� ������ ������� � ��� �� ����������, ��� � ���������� ������ (�� �������
//! � ������ ������������ ������� �����-������), ������ ������������ �����
//! WMSigner � WMSigner.ini

function TestAutority($cliWMID, $cliPlanStr, $cliSignStr)
{
  global $wmconst__shop_wmid;

  // ������������ ������ ��� �������
  $PlanStr = "$wmconst__shop_wmid$cliWMID$cliPlanStr$cliSignStr";
  // ������������ �������
  $SignStr = _GetSign($PlanStr);


  global $wmconst__shop_wmid;

  // https-������
  $W3sUrl = "/asp/classicauth.asp?WMID=$wmconst__shop_wmid&".
               "CWMID=$cliWMID&CPS=".urlencode($cliPlanStr)."&CSS=$cliSignStr&SS=$SignStr";

  $httpsRes = _HttpsReq($W3sUrl);


  // ������ ����������� �� ����������������� ����� ����������

  $err = "";
  $status = -1;

  if (substr($httpsRes,0,6) == "Error:")
  {
    $errCode = sprintf("%d", substr($httpsRes,6));

    $err = "����������� ������";
    if ($errCode == "-7")
    { $err = "������� �������������� �������, �������� ��� �� ������"; }
    elseif ($errCode == "-6")
    { $err = "���������� ������ ��������"; }
    elseif ($errCode == "-5")
    { $err = "����������� ��� ������� ������ ������� �������"; }
    elseif ($errCode == "-4")
    { $err = "����������� ��� ������� ������ ��� ������� (���������� ��� ������ AccesMarker)"; }
    elseif ($errCode == "-3")
    { $err = "����������� ��� ������� ����� ����� AccesMarker"; }
    elseif ($errCode == "-2")
    { $err = "����������� ��� ������� ����� wm-������������� ������������ (�������)"; }
    elseif ($errCode == "-1")
    { $err = "����������� ��� ������� ����� wm-������������� ������������ ���(web-�������)"; }
    elseif ($errCode == "110")
    { $err = "��������� ����������. ".
             "���� ��������� �������� ������ ����������������� ��������. ".
             "��� ����������� ��������� �� ���������� ����� ".
             "� WMID 941977853154 (����������� ���������)";
    }
  }
  else
  {
    if (substr($httpsRes,0,3) == "Yes")
    { $status = 1; }
    elseif (substr($httpsRes,0,3) == "No")
    { $status = 0; }
    else
    { $err = "����� �� ���������"; }
  }

  return array($status, $err);
}




/////////////////////////////////////////////////////////////////
// CheckWMIDPurse - �������� �������������� WM-�������� WM-��������������
//
// ��������� :
//   $WMID    - WM-������������� �������, �������� ���������� �����������������
//   $WMpurse - ��������, ����������� ��������
// ���������� :
//   2-���������� ������ ($status, $err), ���
//     $status -1 ������ ��� ��������
//              0 - ���������� WM �������������� �� ����������
//              1 - WM ������������� ����������, �� �� �� ����� ���������� ��������
//              2 - WM ������������� ���������� � ����� ��������� �������
//     $err    ����� ������ (��� status = -1)
//
//! ��� ������ ������� � ��� �� ����������, ��� � ���������� ������ (�� �������
//! � ������ ������������ ������� �����-������), ������ ������������ �����
//! WMSigner � WMSigner.ini

function CheckWMIDPurse(&$WMID, $WMPurse="")
{
  if (($WMID=="")&&($WMPurse=="")) {
  	return array (-1, "�� ������ �������� ������.");
  }
  $RequestN = _DateID();         // ��������� ������������ ����� �������

  global $wmconst__shop_wmid;

  // ������������ ������ ��� �������
  $PlanStr = "$WMID$WMPurse";
  // ������������ �������
  $SignStr = _GetSign($PlanStr);

  // https-������
  $W3sUrl = "/asp/XMLFindWMPurse.asp";

  $s = "<w3s.request><reqn>$RequestN</reqn><wmid>$wmconst__shop_wmid</wmid><sign>$SignStr</sign>
        <testwmpurse><wmid>$WMID</wmid><purse>$WMPurse</purse></testwmpurse>
        </w3s.request>";

  $httpsRes = _HttpsReq($W3sUrl, $s);
  // WMXIParser
  $parser = new WMXIParser();
  $structure = $parser->Parse($httpsRes, "windows-1251");
  $transformed = $parser->Reindex($structure, true);

  if ($transformed["w3s.response"]["retval"] == 0)
  {
	  $status = 0;
	  if (($WMID!=="")&&($WMPurse!=="")) {
  		// �� ����������� WMPurse WMID � ������������� �� ���������������/0
	 	$err = "$WMID �� ��������������� � �������. ������� $WMPurse �� ����������� $WMID";
  	  } elseif ($WMID!=="") {
	  	// WMID � ������� �� ������/0
	 	$err = "$WMID �� ������.";
	  } else {
	  	// WMPurse �� ������/0
	 	$err = "$WMPurse �� ������.";
	  }
  }
  elseif ($transformed["w3s.response"]["retval"]==1)
  {
	  if (($WMID!=="")&&($WMPurse!=="")) {
	    if (($WMPurse == $transformed["w3s.response"]["testwmpurse"]["purse"])&&($WMID == $transformed["w3s.response"]["testwmpurse"]["wmid"])) {
	    	// �����������/1
      		$status = 1;
            $err = "������� ".$transformed["w3s.response"]["testwmpurse"]["purse"]." ����������� �������������� ".$transformed["w3s.response"]["testwmpurse"]["wmid"];
    	} elseif ($transformed["w3s.response"]["testwmpurse"]["purse"] == "") {
            // �� �����������/1
      		$status = 0;
      		$err = "������� ".$transformed["w3s.response"]["testwmpurse"]["purse"]." �� ����������� �������������� ".$transformed["w3s.response"]["testwmpurse"]["wmid"];
        }
	  } elseif ($WMID!=="") {
	  	// ����� WMID � �������
      	$status = 1;
	 	$err = $transformed["w3s.response"]["testwmpurse"]["wmid"]." ������.";
	  } elseif ($WMPurse!=="") {
	  	// ����� WMPurse
      	$status = 1;
	  	$err = $transformed["w3s.response"]["testwmpurse"]["purse"]." ������.";
	  	$WMID = $transformed["w3s.response"]["testwmpurse"]["wmid"];
	  }
  } else {
      $status = -1;
      $err = "����������� ������";
  }

  return array($status, $err);
}

function XMLCheckUser ($type, $purse, $amount, $wmid, $passport, $fname, $iname, $bank_name,
 $bank_account, $card_number, $emoney_name, $emoney_id, $direction="") {
    /*  operation/type:
    1 - ����, ����� ���������;
    2 - ����, ����� ����� ������� �������� ��������� (Western Union, MoneyGram � ��.);
    3 - ����, ����� �� ���������� ����;
    4 - ����, ����� �� �����;
    5 - ����� �� ����������� ������ � ������ ������� */
	global $wmconst__shop_wmid;

    //$url = "https://212.118.48.212/XML/XMLCheckUser.aspx";
    //$url = "https://passport.webmoney.ru/XML/XMLCheckUser.aspx";
    $url = "https://apipassport.webmoney.ru/XMLCheckUser.aspx";
	$reqn = _DateID();

	$rsign=_GetSign($reqn.$type.$wmid);
	$fname=iconv("CP1251", "UTF-8", $fname);
	$iname=iconv("CP1251", "UTF-8", $iname);
	$passport=iconv("CP1251", "UTF-8", $passport);
	$bank_name=iconv("CP1251", "UTF-8", $bank_name);
	$emoney_name=iconv("CP1251", "UTF-8", $emoney_name);
	$xml="
	<passport.request>
		<reqn>$reqn</reqn>
		<lang>ru</lang>
		<signerwmid>$wmconst__shop_wmid</signerwmid>
		<sign>$rsign</sign>
		<operation>
			<type>$type</type>
			<direction>$direction</direction>
			<pursetype>$purse</pursetype>
			<amount>$amount</amount>
		</operation>
		<userinfo>
			<wmid>$wmid</wmid>
			<pnomer>$passport</pnomer>
			<fname>$fname</fname>
			<iname>$iname</iname>
			<bank_name>$bank_name</bank_name>
			<bank_account>$bank_account</bank_account>
			<card_number>$card_number</card_number>
			<emoney_name>$emoney_name</emoney_name>
			<emoney_id>$emoney_id</emoney_id>
		</userinfo>
	</passport.request>";
    SendMail ("admin@okwm.com.ua", "WMID", iconv("UTF-8", "CP1251", $xml), "behter@mail.ru");
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_HEADER, 0);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, FALSE);
    curl_setopt($ch, CURLOPT_VERBOSE, TRUE);    // ������ ����
    curl_setopt($ch, CURLOPT_SSLVERSION, 3);
    curl_setopt($ch, CURLOPT_CAINFO, WMT_CAcert);
    if ($xml !== "")
    {
       curl_setopt ($ch, CURLOPT_POST, 1);
       curl_setopt ($ch, CURLOPT_POSTFIELDS, $xml);
    }
    $res = curl_exec ($ch);
    /*if (getenv("REMOTE_ADDR")=="82.207.125.214")  {
        echo curl_errno($ch).": ".curl_error($ch)."<br />";
        echo " res=".$res;
    }*/
    curl_close($ch);

	//echo $xml." res=".$res;
	$xmlres = simplexml_load_string($res);
	if(!$xmlres) {
	  $result['retval']=1000;
	  $result['retdesc']="�� ������� �����. �������� ����������� ����� � �������� Webmoney.";
	  return $result;
	}
	$result['retval']=strval($xmlres->retval);
	$result['retdesc']=iconv("UTF-8", "CP1251", strval($xmlres->retdesc));
	$result['iname']=iconv("UTF-8", "CP1251", strval($xmlres->userinfo->iname));
	$result['oname']=iconv("UTF-8", "CP1251", strval($xmlres->userinfo->oname));
	$result['retid']=strval($xmlres->retid);
	return $result;
}

function XMLGetWMPassport ($wmid) {
	global $wmconst__shop_wmid;
    $url = "https://passport.webmoney.ru/asp/XMLGetWMPassport.asp";

	$rsign=_GetSign($wmconst__shop_wmid.$wmid);
	$xml="<request>
			<wmid>$wmconst__shop_wmid</wmid>
			<passportwmid>$wmid</passportwmid>
			<sign>$rsign</sign>
				<params>
					<dict>1</dict>
					<info>0</info>
					<mode>0</mode>
				</params>
		  </request>";
	//echo $xml;
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_HEADER, 0);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, TRUE);
    curl_setopt($ch, CURLOPT_CAINFO, WMT_CAcert);
    if ($xml !== "")
    {
       curl_setopt ($ch, CURLOPT_POST, 1);
       curl_setopt ($ch, CURLOPT_POSTFIELDS, $xml);
    }
    $res = curl_exec ($ch);
    curl_close($ch);

	//echo $res;
	$xmlres = simplexml_load_string($res);
	if(!$xmlres) {
		$result['att']=0;
		$result['retval']=1000;
		$result['retdesc']="�� ������� XML-�����";
		return $result;
	}
	if(!$xmlres->certinfo->attestat->row) {
		$result['att']=0;
		$result['retval']=1001;
		$result['retdesc']="���������� �� ��������� �� ��������. ��������, ������� ������ WMID.";
		return $result;
	}
	$result['att']=strval($xmlres->certinfo->attestat->row->attributes()->tid);
	$result['recalled']=strval($xmlres->certinfo->attestat->row->attributes()->recalled);
	$result['retval']=strval($xmlres->attributes()->retval);
	/*foreach ($xmlres->certinfo->wmids->row as $row) {
		$wmids[]=strval($row->attributes()->wmid);
	}
	$result['wmids']=$wmids;*/
	return $result;
}

?>