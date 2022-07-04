#include "stdafx.h"
#include "KrGcp.h"
#include "authorization\CCSnHelper.h"
PyObject *KrGcp::CLASS_GCP = nullptr;


KrGcp::KrGcp(const QString& xml_path, 
	const QString& sift_photo_input, const QString& compute_line_input,
	const QString& sift_photo_output, const QString& compute_line_output,
	const QString& update_photos_input, const QString& update_photos_output,
	const QString& license_url, const QString& license_code,
	const QString& validate_return_output,const QString& update_photos_tp_input,
	const QString& update_photos_tp_output )
	: m_xml_path(xml_path)
	, m_sift_photo_input(sift_photo_input)
	, m_compute_line_input(compute_line_input)
	, m_sift_photo_output(sift_photo_output)
	, m_compute_line_output(compute_line_output)
	, m_update_photos_input(update_photos_input)
	, m_update_photos_output(update_photos_output)
	, m_license_url(license_url)
	, m_license_code(license_code)
	, m_validate_return_output(validate_return_output)
	, m_update_photos_tp_input(update_photos_tp_input)
	, m_update_photos_tp_output(update_photos_tp_output)
{
	PyObject* pArgs = PyTuple_New(12);
	PyTuple_SetItem(pArgs, 0, Py_BuildValue("s", m_xml_path.toStdString().c_str()));
	PyTuple_SetItem(pArgs, 1, Py_BuildValue("s", m_sift_photo_input.toStdString().c_str()));
	PyTuple_SetItem(pArgs, 2, Py_BuildValue("s", m_compute_line_input.toStdString().c_str()));
	PyTuple_SetItem(pArgs, 3, Py_BuildValue("s", m_sift_photo_output.toStdString().c_str()));
	PyTuple_SetItem(pArgs, 4, Py_BuildValue("s", m_compute_line_output.toStdString().c_str()));
	PyTuple_SetItem(pArgs, 5, Py_BuildValue("s", m_update_photos_input.toStdString().c_str()));
	PyTuple_SetItem(pArgs, 6, Py_BuildValue("s", m_update_photos_output.toStdString().c_str()));
	PyTuple_SetItem(pArgs, 7, Py_BuildValue("s", m_license_url.toStdString().c_str()));
	PyTuple_SetItem(pArgs, 8, Py_BuildValue("s", m_license_code.toStdString().c_str()));
	PyTuple_SetItem(pArgs, 9, Py_BuildValue("s", m_validate_return_output.toStdString().c_str()));
	PyTuple_SetItem(pArgs, 10, Py_BuildValue("s", m_update_photos_tp_input.toStdString().c_str()));
	PyTuple_SetItem(pArgs, 11, Py_BuildValue("s", m_update_photos_tp_output.toStdString().c_str()));

	OBJECT_GPC = PyObject_CallObject(CLASS_GCP, pArgs);  //PyObject_CallObject /PyObject_CallFunctionObjArgs
	PyErr_Print();
	if (OBJECT_GPC == nullptr)
	{
		LOGFMTI("KrGcp::initialize() OBJECT_GPC is NULL");
		return;
	}

	FUNC_VERIFY_LICENSE = PyObject_GetAttrString(OBJECT_GPC, "setLicense");
	FUNC_SIFT_PHOTOS = PyObject_GetAttrString(OBJECT_GPC, "sift_photos");
	FUNC_COMPUTE_LINE = PyObject_GetAttrString(OBJECT_GPC, "comput_line");
	FUNC_UPDATE_PHOTOS = PyObject_GetAttrString(OBJECT_GPC, "update_Photos");
	FUNC_UPDATE_PHOTOS_TP = PyObject_GetAttrString(OBJECT_GPC, "update_Photos_tp");
	
}


KrGcp::~KrGcp()
{
	// Py_Finalize();
}


bool KrGcp::initialize(const QString & _cc_lic)
{
	Py_Initialize();
	PyRun_SimpleString("import sys,os");
#if 0
	Py_SetPythonHome(L"D:/Git/renderfarm-dayan/deps/common/python36");
	Py_Initialize();
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('D:/Git/renderfarm-dayan/deps/common/python36')");
	PyRun_SimpleString("sys.path.append('D:/Git/renderfarm-dayan/dist/windows/bin')");   
	PyErr_Print();
#endif
	
	//	putenv("CC_CENTER=E:\\CC_LIC\\CCSDKOEM-10.19.0.200.x64-en_20220302");

	QString dir_path = QCoreApplication::applicationDirPath();
	string s = dir_path.toStdString();
	string s_bin = s + "/bin";

	char path[1024];
	char path1[128] = "os.environ['CC_CENTER']='";
	char path2[256] = { "/0" };
	for (int i = 0; i < s.length(); i++)
	{
		path2[i] = s[i];
	}
	char path3[8] = "'";
	sprintf_s(path, "%s%s%s", path1, path2, path3);

#if 1
	
	QString cmd = QString("os.add_dll_directory('%1/bin')").arg(dir_path);
	PyRun_SimpleString(cmd.toStdString().c_str());
	
#endif 

#if 0
	char bin_path[1024];
	char bin_path1[128] = "sys.path.append('";
	char bin_path2[256] = { "/0" };
	for (int i = 0; i < s_bin.length(); i++)
	{
		bin_path2[i] = s_bin[i];
	}
	char bin_path3[8] = "')";
	sprintf_s(bin_path, "%s%s%s", bin_path1, bin_path2, bin_path3);
	//	PyRun_SimpleString(bin_path);

#endif 

#if 0
	std::string env = getenv("PATH");
	env = env + ";" + s_bin;
	std::string newEnv = "PATH=" + env;
	putenv(newEnv.c_str());
	env = getenv("PATH");
#endif

#if 0
	QString value = dir_path + "/bin";
	QString nValue = QDir::toNativeSeparators(value);
	QSettings set(QLatin1String("\\HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Control\\Session Manager\\Environment"), QSettings::NativeFormat);
	QString curValue = set.value(QLatin1String("path")).toString();
	if (!curValue.split(QLatin1String(";")).contains(nValue)) {
		curValue.append(QLatin1String(";"));
		curValue.append(nValue);
		set.setValue(QLatin1String("path"), curValue);
		set.sync();
	}

#endif

	char *p;
	if (!(p = getenv("CC_CENTER")))
	{
		PyRun_SimpleString("import os");
		PyRun_SimpleString(path);
	}

	char snBuff[256] = { "/0" };
//	GetLocalSn(snBuff, 256);
	if (!GetLocalSn(snBuff, 256))
	{

	}
//#define test
#ifndef test
	int res = ActiveSn(_cc_lic.toStdString().c_str());
#else
	string string_lic = "OC6P2Z6AV43KTJDB";
	int res = ActiveSn(string_lic.c_str());
#endif
	


	if (res)
	{
		LOGFMTI(" KrGcp::initialize  CCSDK license code is error! code=%d ", res);
		return false;
	}
	LOGFMTI("KrGcp::initialize  cc license code is OK! code=%d ", res);

	PyRun_SimpleString("print(sys.path)");

	PyObject* MODULE_KR_SDK_GCP = PyImport_ImportModule("KR_SDK_GCP_EXEC");
	PyErr_Print();
	if (MODULE_KR_SDK_GCP == NULL) {
		LOGFMTI("KrGcp::initialize() KR_SDK_GCP_EXEC is NULL");	
		PyErr_Print();

		return false;
	}



	CLASS_GCP = PyObject_GetAttrString(MODULE_KR_SDK_GCP, "GCPExec");
	if (CLASS_GCP == NULL)
	{
		LOGFMTI("KrGcp::initialize() CLASS_GCP is NULL");
		return false;
	}
	return true;
}

void KrGcp::siftPhotos()
{
	PyObject_CallObject(FUNC_SIFT_PHOTOS, NULL);
	if (QFile::exists(m_sift_photo_output))
		LOGFMTI("KrGcp::siftPhotos() sift_photo_output is OK");
	else
		LOGFMTI("KrGcp::siftPhotos() sift_photo_output is NULL");
}

void KrGcp::computeLine()
{
	PyObject_CallObject(FUNC_COMPUTE_LINE, NULL);

	if (QFile::exists(m_compute_line_output))
		LOGFMTI("KrGcp::computeLine() compute_line_output is OK");
	else
		LOGFMTI("KrGcp::computeLine() compute_line_output is NULL");
}

void KrGcp::updataPhotos()
{
	PyObject_CallObject(FUNC_UPDATE_PHOTOS, NULL);
	if (QFile::exists(m_update_photos_output))
		LOGFMTI("KrGcp::updataPhotos() update_photos_output is OK");
	else
		LOGFMTI("KrGcp::updataPhotos() update_photos_output is NULL");
}

void KrGcp::verifylicense()
{
	PyObject_CallObject(FUNC_VERIFY_LICENSE, NULL);
	if (QFile::exists(m_validate_return_output))
		LOGFMTI("KrGcp::verifylicense() verifylicense is OK");
	else
		LOGFMTI("KrGcp::verifylicense() verifylicense is NULL");
}

PyObject * KrGcp::getObjectGCP()
{
	return OBJECT_GPC;
}

void KrGcp::updataPhotosTP()
{
	PyObject_CallObject(FUNC_UPDATE_PHOTOS_TP, NULL);
	if (QFile::exists(m_update_photos_tp_output))
		LOGFMTI("KrGcp::updataPhotosTP() updataPhotosTP is OK");
	else
		LOGFMTI("KrGcp::updataPhotosTP() updataPhotosTP is NULL");
}
