<script language="PHP">
#--------------------------------------------------------------------
# OKWM WHITH WEBMONEY page.
# Copyright by Yaroslav Behter (behter@hoodrook.com), (c) 2010.
#--------------------------------------------------------------------
# Requires /lib/utility.asp
#--------------------------------------------------------------------

require_once("${HTTP_SERVER_VARS['DOCUMENT_ROOT']}/lib/utility.asp");
$http = DOMAIN_HTTP;

    // SEO strings
    $title = GetSEO("c3c5d62cecbb014709481767d39a64e7");
    if ($title=="") $title = "���������, ������, �������� ( wmz, wmu, wmr, wme) � ������� | OKWM - �������� ���, ���, ���, ��� ��� ��������";
    $Description = GetSEO("017240a89bf5a7ccf697e220e268fdfc");
    if ($Description=="") $description = "������� � ������ �������� WebMoney WMZ WMR WMU WME, Ukash, Pokerstars, Betfair, �������� � �����, ������, ���������������";
    $Keywords = GetSEO("a5519620408ddfea7d6b2a43da73f4ad");
    if ($Keywords=="") $keywords = "Webmoney, ������� WebMoney, ����� webmoney, �������, ������ ������� WMZ, ����� WMZ, wmz �� �������, WMZ �� ��������, WMZ, WMR, WMU, WME, ��������, ����������� ������";


HTMLHead ($title, $Keywords, $Description);
echo <<<EOT

	 <tr><td></td><td id="phl"><img width="1" height="4" alt="" src="$http/img/0.gif"/></td>
	     <td id="phr"><img width="210" height="4" alt="" src="$http/img/0.gif" /></td><td></td>
	     <td rowspan="5" class="right_banners">
EOT;
    if (!isset($HTTP_SERVER_VARS['HTTPS'])) EchoBaners ();
    echo <<<EOT
	 </td>
	 </tr>
	 <tr><td class="lsh"></td><td id="pbl">
       <h1>OKWM - ������ ����������� �������� ����� WebMoney</h1>
	 </td>
	 <td id="pbr">&nbsp;
    </td><td class="rsh"></td>
	 </tr>
     <tr height="600"><td class="lsh"></td><td id="content" colspan="2">
      <div id="help">
        <h3 class="h3_title">��������� ����������� �������� ����� WebMoney!</h3>

		<p>��� ��������, ������� ���������� � ������ ����� ���������� ���������� ��������� <a href="http://ufcenter.com.ua/" target="_blank">&laquo;���������� ������������� �����&raquo;</a> �������� ����������� ��� � ���������� ��������...</p>

		<p>� ����� ������� �� ������ <a href="http://www.okwm.com.ua/contacts/map_kiev.asp">������</a> ���������� ����������� �������� �����, ������� �������� ����� ����� ��� ������� �������� � ����������. ��� ����������� ������ � �����, �� �������� ����� �� �����, ������� ����������� ����������� �� ����� <a href="http://www.okwm.com.ua">OKWM</a>.</p>

		<p>�������� �� ������� ����������� ����� �� ������ �������� ��������� �������:</p>

		<p>1 ���: �� �������� �� ���� okwm.com.ua ��������� ����� �������� ������ �� ������� �� ����������� ������, � ��� �� ����� ����, � ������� ������ ��������� ������������ ��� �����.<br />
		2 ���: ��������� �� ������, ������� ������ �� ����� <a href="http://www.okwm.com.ua">OKWM</a>, ����� ���������� �������� � ���������� � ������� ��� �����. ��� ���������� ������ � ��������� ���.<br />
		3 ���: ���������� �������� &laquo;���&raquo; ���������� ������� WMU � ������ �������� ������� � �������.<br />
		4 ���: ������ ���������� WMU ������� <a href="http://www.okwm.com.ua">OKWM</a>, ����� ���� �� ��������� ���������� ������.</p>

		<p>���� �� ������ ��������� ������� ������ ����������� ����� �� ������, ������� �� �������� ����������� ������� ������� (�������, ����, ����� � ��.) ��� ���������� ������� ��������� ����:</p>

		<p>1 ���: �� �������� �� ���� okwm.com.ua ��������� ����� �������� ������ �� ������� �� ����������� ������, � ��� �� � ����� ������ ��� ���������� ������ �����.<br />
		2 ���: ��������� �� ������, ������� ������ �� ����� <a href="http://www.okwm.com.ua">OKWM</a>, ����������� �������� ������ �� ������ � �������� ������ � ��������� ���.<br />
		3 ���: � ����� ���������� �������� ���������� ���� �����, ����������� ����� �������� � ��������� ��������� � ������� WMU �� ������.<br />
		3 ���: ���������� �������� &laquo;���&raquo; ���������� ������� WMU � ������ �������� ������� � �������.<br />
		4 ���: ������ ���������� WMU ������� <a href="http://www.okwm.com.ua">OKWM</a>, ����� ���� �� ��������� ���������� ������.</p>

		<p>��� �������� ������ ����������� �����, �� ��������� �������� ����� ����� ��, ������ ��������.
		����� ��������������:</p>

        <img src="/img/okwm_ufc.png" alt="����� �������� Wemboney">

		<h3 class="h3_title">��������!</h3>
		<p>���������� ��� � ���������� �������� - ��� ������������� � ������ ����� ������������!</p>
		<p>�� ���� �������� � ��������� ���������� � ����� � ������ WebMoney ����������� � �������������� ������� <a href="http://www.okwm.com.ua/contacts.asp">OKWM</a>.</p>


      </div>
   </td><td class="rsh"></td></tr>
EOT;

HTMLTail();
</script>