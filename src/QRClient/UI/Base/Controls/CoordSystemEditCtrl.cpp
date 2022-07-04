#include "stdafx.h"
#include "CoordSystemEditCtrl.h"
#include "SRS/srswindow.h"

CoordSystemEditCtrl::CoordSystemEditCtrl(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    ui.label_ctrlpoint->setStyleSheet("QLabel{color:#eeeeee;}");
}

CoordSystemEditCtrl::~CoordSystemEditCtrl()
{
	qDebug() << "~CoordSystemEditCtrl()";
}

void CoordSystemEditCtrl::hideCoordImage()
{
    ui.label_6->hide();
}

void CoordSystemEditCtrl::setLabelText(const QString& text)
{
    ui.label_3->setText(text);
}

void CoordSystemEditCtrl::setLableFixWidth(int width)
{
    ui.label_3->setFixedWidth(width);
}

void CoordSystemEditCtrl::initCoordSystem()
{
    QSettings settings(m_regKey, QSettings::NativeFormat);
    auto keys = settings.allKeys();
    if (keys.isEmpty()) {
        ui.label_ctrlpoint->setText("WGS 84 - World Geodetic System 1984 (EPSG:4326)");
        ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4326");
    } else {
        ui.label_ctrlpoint->setText(settings.value(keys[0]).toString());
        ui.label_ctrlpoint->setProperty("sysDefine", keys[0]);
    }
}

void CoordSystemEditCtrl::initCoordSystem(const double & lon,  const double& lat)
{
#ifdef FOXRENDERFARM
	//国外数据
	QSettings settings(m_regKey, QSettings::NativeFormat);
	auto keys = settings.allKeys();
	if (keys.isEmpty()) {
		ui.label_ctrlpoint->setText("WGS 84 - World Geodetic System 1984 (EPSG:4326)");
		ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4326");
	}
	else {
		if (lat > 0.0)
		{
			if (lon >= -180 && lon < -174)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 1N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32601");
			}
			else if (lon >= -174 && lon < -168)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 2N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32602");
			}
			else if (lon >= -168 && lon < -162)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 3N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32603");
			}
			else if (lon >= -162 && lon < -156)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 4N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32604");
			}
			else if (lon >= -156 && lon < -150)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 5N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32605");
			}
			else if (lon >= -150 && lon < -144)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 6N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32606");
			}
			else if (lon >= -144 && lon < -138)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 7N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32607");
			}
			else if (lon >= -138 && lon < -132)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 8N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32608");
			}
			else if (lon >= -132 && lon < -126)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 9N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32609");
			}
			else if (lon >= -126 && lon < -120)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 10N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32610");
			}
			else if (lon >= -120 && lon < -114)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 11N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32611");
			}
			else if (lon >= -114 && lon < -108)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 12N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32612");
			}
			else if (lon >= -108 && lon < -102)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 13N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32613");
			}
			else if (lon >= -102 && lon < -96)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 14N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32614");
			}
			else if (lon >= -96 && lon < -90)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 15N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32615");
			}
			else if (lon >= -90 && lon < -84)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 16N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32616");
			}
			else if (lon >= -84 && lon < -78)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 17N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32617");
			}
			else if (lon >= -78 && lon < -72)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 18N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32618");
			}
			else if (lon >= -72 && lon < -66)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 19N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32619");
			}
			else if (lon >= -66 && lon < -60)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 20N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32620");
			}
			else if (lon >= -60 && lon < -54)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 21N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32621");
			}
			else if (lon >= -54 && lon < -48)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 22N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32622");
			}
			else if (lon >= -48 && lon < -42)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 23N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32623");
			}
			else if (lon >= -42 && lon < -36)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 24N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32624");
			}
			else if (lon >= -36 && lon < -30)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 25N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32625");
			}
			else if (lon >= -30 && lon < -24)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 26N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32626");
			}
			else if (lon >= -24 && lon < -18)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 27N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32627");
			}
			else if (lon >= -18 && lon < -12)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 28N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32628");
			}
			else if (lon >= -12 && lon < -6)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 29N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32629");
			}
			else if (lon >= -6 && lon < 0)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 30N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32630");
			}
			else if (lon >= 0 && lon < 6)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 31N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32631");
			}
			else if (lon >= 6 && lon < 12)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 32N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32632");
			}
			else if (lon >= 12 && lon < 18)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 33N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32633");
			}
			else if (lon >= 18 && lon < 24)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 34N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32634");
			}
			else if (lon >= 24 && lon < 30)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 35N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32635");
			}
			else if (lon >= 30 && lon < 36)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 36N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32636");
			}
			else if (lon >= 36 && lon < 42)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 37N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32637");
			}
			else if (lon >= 42 && lon < 48)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 38N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32638");
			}
			else if (lon >= 48 && lon < 54)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 39N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32639");
			}
			else if (lon >= 54 && lon < 60)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 40N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32640");
			}
			else if (lon >= 60 && lon < 66)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 41N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32641");
			}
			else if (lon >= 66 && lon < 72)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 42N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32642");
			}
			else if (lon >= 72 && lon < 78)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 43N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32643");
			}
			else if (lon >= 78 && lon < 84)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 44N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32644");
			}
			else if (lon >= 84 && lon < 90)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 45N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32645");
			}
			else if (lon >= 90 && lon < 96)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 46N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32646");
			}
			else if (lon >= 96 && lon < 102)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 47N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32647");
			}
			else if (lon >= 102 && lon < 108)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 48N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32648");
			}
			else if (lon >= 108 && lon < 114)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 49N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32649");
			}
			else if (lon >= 114 && lon < 120)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 50N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32650");
			}
			else if (lon >= 120 && lon < 126)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 51N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32651");
			}
			else if (lon >= 126 && lon < 132)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 52N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32652");
			}
			else if (lon >= 132 && lon < 138)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 53N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32653");
			}
			else if (lon >= 138 && lon < 144)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 54N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32654");
			}
			else if (lon >= 144 && lon < 150)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 55N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32655");
			}
			else if (lon >= 150 && lon < 156)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 56N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32656");
			}
			else if (lon >= 156 && lon < 162)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 57N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32657");
			}
			else if (lon >= 162 && lon < 168)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 58N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32658");
			}
			else if (lon >= 168 && lon < 174)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 59N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32659");
			}
			else if (lon >= 174 && lon < 180)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 60N");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32660");
			}
			else
			{
				ui.label_ctrlpoint->setText("WGS 84 - World Geodetic System 1984 (EPSG:4326)");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4326");
			}
		}
		else if (lat <0.0)
		{
			if (lon >= -180 && lon < -174)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 1S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32701");
			}
			else if (lon >= -174 && lon < -168)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 2S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32702");
			}
			else if (lon >= -168 && lon < -162)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 3S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32703");
			}
			else if (lon >= -162 && lon < -156)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 4S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32704");
			}
			else if (lon >= -156 && lon < -150)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 5S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32705");
			}
			else if (lon >= -150 && lon < -144)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 6S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32706");
			}
			else if (lon >= -144 && lon < -138)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 7S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32707");
			}
			else if (lon >= -138 && lon < -132)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 8S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32708");
			}
			else if (lon >= -132 && lon < -126)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 9S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32709");
			}
			else if (lon >= -126 && lon < -120)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 10S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32710");
			}
			else if (lon >= -120 && lon < -114)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 11S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32711");
			}
			else if (lon >= -114 && lon < -108)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 12S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32712");
			}
			else if (lon >= -108 && lon < -102)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 13S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32713");
			}
			else if (lon >= -102 && lon < -96)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 14S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32714");
			}
			else if (lon >= -96 && lon < -90)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 15S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32715");
			}
			else if (lon >= -90 && lon < -84)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 16S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32716");
			}
			else if (lon >= -84 && lon < -78)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 17S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32717");
			}
			else if (lon >= -78 && lon < -72)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 18S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32718");
			}
			else if (lon >= -72 && lon < -66)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 19S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32719");
			}
			else if (lon >= -66 && lon < -60)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 20S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32720");
			}
			else if (lon >= -60 && lon < -54)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 21S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32721");
			}
			else if (lon >= -54 && lon < -48)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 22S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32722");
			}
			else if (lon >= -48 && lon < -42)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 23S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32723");
			}
			else if (lon >= -42 && lon < -36)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 24S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32724");
			}
			else if (lon >= -36 && lon < -30)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 25S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32725");
			}
			else if (lon >= -30 && lon < -24)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 26S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32726");
			}
			else if (lon >= -24 && lon < -18)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 27S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32727");
			}
			else if (lon >= -18 && lon < -12)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 28S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32728");
			}
			else if (lon >= -12 && lon < -6)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 29S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32729");
			}
			else if (lon >= -6 && lon < 0)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 30S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32730");
			}
			else if (lon >= 0 && lon < 6)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 31S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32731");
			}
			else if (lon >= 6 && lon < 12)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 32S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32732");
			}
			else if (lon >= 12 && lon < 18)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 33S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32733");
			}
			else if (lon >= 18 && lon < 24)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 34S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32734");
			}
			else if (lon >= 24 && lon < 30)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 35S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32735");
			}
			else if (lon >= 30 && lon < 36)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 36S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32736");
			}
			else if (lon >= 36 && lon < 42)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 37S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32737");
			}
			else if (lon >= 42 && lon < 48)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 38S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32738");
			}
			else if (lon >= 48 && lon < 54)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 39S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32739");
			}
			else if (lon >= 54 && lon < 60)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 40S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32740");
			}
			else if (lon >= 60 && lon < 66)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 41S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32741");
			}
			else if (lon >= 66 && lon < 72)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 42S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32742");
			}
			else if (lon >= 72 && lon < 78)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 43S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32743");
			}
			else if (lon >= 78 && lon < 84)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 44S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32744");
			}
			else if (lon >= 84 && lon < 90)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 45S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32745");
			}
			else if (lon >= 90 && lon < 96)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 46S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32746");
			}
			else if (lon >= 96 && lon < 102)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 47S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32747");
			}
			else if (lon >= 102 && lon < 108)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 48S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32748");
			}
			else if (lon >= 108 && lon < 114)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 49S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32749");
			}
			else if (lon >= 114 && lon < 120)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 50S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32750");
			}
			else if (lon >= 120 && lon < 126)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 51S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32751");
			}
			else if (lon >= 126 && lon < 132)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 52S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32752");
			}
			else if (lon >= 132 && lon < 138)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 53S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32753");
			}
			else if (lon >= 138 && lon < 144)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 54S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32754");
			}
			else if (lon >= 144 && lon < 150)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 55S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32755");
			}
			else if (lon >= 150 && lon < 156)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 56S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32756");
			}
			else if (lon >= 156 && lon < 162)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 57S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32757");
			}
			else if (lon >= 162 && lon < 168)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 58S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32758");
			}
			else if (lon >= 168 && lon < 174)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 59S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32759");
			}
			else if (lon >= 174 && lon < 180)
			{
				ui.label_ctrlpoint->setText("WGS 84 / UTM zone 60S");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:32760");
			}
			else
			{
				ui.label_ctrlpoint->setText("WGS 84 - World Geodetic System 1984 (EPSG:4326)");
				ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4326");
			}
		}
		else
		{
			ui.label_ctrlpoint->setText("WGS 84 - World Geodetic System 1984 (EPSG:4326)");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4326");
		}
	}

#else
	//国内数据
	QSettings settings(m_regKey, QSettings::NativeFormat);
	auto keys = settings.allKeys();
	if (keys.isEmpty()) {
		ui.label_ctrlpoint->setText("WGS 84 - World Geodetic System 1984 (EPSG:4326)");
		ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4326");
	}
	else {
		if (lon >= 73.5&& lon < 76.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 75E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4534");
		}
		else if (lon >= 76.5&& lon < 79.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 78E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4535");
		}
		else if (lon >= 79.5&& lon < 82.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 81E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4536");
		}
		else if (lon >= 82.5&& lon < 85.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 84E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4537");
		}
		else if (lon >= 85.5&& lon < 88.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 87E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4538");
		}
		else if (lon >= 88.5&& lon < 91.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 90E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4539");
		}
		else if (lon >= 91.5&& lon < 94.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 93E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4540");
		}
		else if (lon >= 94.5&& lon < 97.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 96E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4541");
		}
		else if (lon >= 97.5&& lon < 100.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 99E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4542");
		}
		else if (lon >= 100.5&& lon < 103.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 102E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4543");
		}
		else if (lon >= 103.5&& lon < 106.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 105E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4544");
		}
		else if (lon >= 106.5&& lon < 109.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 108E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4545");
		}
		else if (lon >= 109.5&& lon < 112.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 111E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4546");
		}
		else if (lon >= 112.5&& lon < 115.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 114E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4547");
		}
		else if (lon >= 115.5&& lon < 118.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 117E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4548");
		}
		else if (lon >= 118.5&& lon < 121.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 120E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4549");
		}
		else if (lon >= 121.5&& lon < 124.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 123E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4550");
		}
		else if (lon >= 124.5&& lon < 127.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 126E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4551");
		}
		else if (lon >= 127.5&& lon < 130.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 129E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4552");
		}
		else if (lon >= 130.5&& lon < 133.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 132E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4553");
		}
		else if (lon >= 133.5&& lon <= 136.5)
		{
			ui.label_ctrlpoint->setText("CGCS2000 / 3-degree Gauss-Kruger CM 125E");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4554");
		}
		else
		{
			ui.label_ctrlpoint->setText("WGS 84 - World Geodetic System 1984 (EPSG:4326)");
			ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4326");
		}
	}
#endif	


}

void CoordSystemEditCtrl::on_btnEditCoord_clicked()
{
	// LOGFMTI("CoordSystemEditCtrl::btnEditCoord() regkey %s, ctrl pointer %d", qPrintable(m_regKey), (void*)this);

    SRSWindow system(ui.label_ctrlpoint->property("sysDefine").toString(), m_regKey);

    if (system.exec() != QDialog::Accepted)
        return;
    auto coordSystem = system.getCoordinateSystem();

    ui.label_ctrlpoint->setText(coordSystem.sysFullName);
    ui.label_ctrlpoint->setProperty("sysDefine", coordSystem.sysDefine);
}

QString CoordSystemEditCtrl::getCoordSystemValue()
{
	// LOGFMTI("CoordSystemEditCtrl::getCoordSystemValue() regkey %s, ctrl pointer %d", qPrintable(m_regKey), (void*)this);
    return ui.label_ctrlpoint->property("sysDefine").toString();
}

QString CoordSystemEditCtrl::getCoordSystem()
{
    return ui.label_ctrlpoint->text();
}

void CoordSystemEditCtrl::initCtrl(const QString& key)
{
    m_regKey = QString("%1%2").arg(COORD_REF_SYSTEM_KEY).arg(key);
    initCoordSystem();
	// LOGFMTI("CoordSystemEditCtrl::initCtrl() regkey %s, ctrl pointer %d", qPrintable(m_regKey), (void*)this);
}

void CoordSystemEditCtrl::initCtrl(const QString& key, const double& lon ,const double& lat)
{
	m_regKey = QString("%1%2").arg(COORD_REF_SYSTEM_KEY).arg(key);
	initCoordSystem(lon ,lat);
}

void CoordSystemEditCtrl::setCoordSystemValue(const QString& v)
{
    if (v.isEmpty()) {
        ui.label_ctrlpoint->setText("WGS 84 - World Geodetic System 1984 (EPSG:4326)");
        ui.label_ctrlpoint->setProperty("sysDefine", "EPSG:4326");
        return;
    }
    ui.label_ctrlpoint->setText(v);
    ui.label_ctrlpoint->setProperty("sysDefine", v);
}