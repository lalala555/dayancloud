#pragma once

#include <cstdlib> 
#pragma push_macro("slots")  //解决Python 和 Qt 的关键词 slots 冲突
#undef slots
#include <Python.h>
#pragma pop_macro("slots")

#pragma execution_character_set("utf-8")

class KrGcp
{
public:

	KrGcp(const QString& xml_path,
		const QString& sift_photo_input, const QString& compute_line_input,
		const QString& sift_photo_output, const QString& compute_line_output,
		const QString& update_photos_input, const QString& update_photos_output,
	    const QString& license_url ,const QString& license_code,
		const QString& validate_return_output,const QString& update_photos_tp_input,
		const QString& update_photos_tp_output);
	~KrGcp();

	static bool initialize(const QString & _cc_lic);  //ccSDK 许可码
	PyObject * getObjectGCP();

	void siftPhotos();
	void computeLine();
	void updataPhotos();
	void verifylicense();
	void updataPhotosTP();

private:
	static PyObject *CLASS_GCP;
	PyObject *OBJECT_GPC;
	PyObject *FUNC_SIFT_PHOTOS;
	PyObject *FUNC_COMPUTE_LINE;
	PyObject *FUNC_UPDATE_PHOTOS;
	PyObject *FUNC_VERIFY_LICENSE;
	PyObject *FUNC_UPDATE_PHOTOS_TP;


	QString m_xml_path;
	QString m_sift_photo_input;
	QString m_compute_line_input;
	QString m_sift_photo_output;
	QString m_compute_line_output;
	QString m_update_photos_input;
	QString m_update_photos_output;
	QString m_license_url;
	QString m_license_code;      //瞰融sdk许可码
	QString m_validate_return_output;
	QString m_update_photos_tp_input;
	QString m_update_photos_tp_output;

};

