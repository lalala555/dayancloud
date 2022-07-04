#pragma once

#include <osg/Geode>     
#include <osg/Geometry>  
#include <osg/MatrixTransform>
#include <osg/Group>
#include <osg/StateSet>
#include <osg/StateAttribute>
#include <osg/ShadeModel>
#include <osg/CullFace>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/Point>

#include <osg/Depth>
#include <osg/TexMat>
#include <osg/TexGen>
#include <osg/TexEnv>
#include <osg/TexEnvCombine>
#include <osg/TextureCubeMap>
#include <osg/VertexProgram>
#include <osg/ShapeDrawable>
#include <osg/Camera>
#include <osg/AutoTransform>

#include <osg/io_utils>
#include <osgText/Text>
#include <sstream>


#include <iostream>
#include <QFile>
#include <QTextStream>
#include <vector>
#include <map>
#include <qmap.h>


struct osgLasPoints
{
	double x;
	double y;
	double z;
	short  intensity;
	double gpstime;
	float r;
	float g;
	float b;
	char classification;
};

struct JsonPoints
{
	double x;
	double y;
	double z;
	double r;
	double g;
	double b;
};

#define  CIRCLE_RADIANS  6.283185307179586476925286766559

#define PI 3.14159265359
#define RAD2DEG(x) (x)*180.0/PI
#define DEG2RAD(x) (x)*PI/180.0
static const double WGS84_A = 6378137.0;      // major axis
static const double WGS84_E = 0.0818191908;   // first eccentricity


osg::Vec3 ECEF2LLA(osg::Vec3 cur_ecef) {
	double x = cur_ecef.x();
	double y = cur_ecef.y();
	double z = cur_ecef.z();
	const double b = sqrt(WGS84_A * WGS84_A * (1 - WGS84_E * WGS84_E));
	const double ep = sqrt((WGS84_A * WGS84_A - b * b) / (b * b));
	const double p = hypot(x, y);
	const double th = atan2(WGS84_A * z, b * p);
	const double lon = atan2(y, x);
	const double lat = atan2((z + ep * ep * b * pow(sin(th), 3)), (p - WGS84_E * WGS84_E * WGS84_A * pow(cos(th), 3)));
	const double N = WGS84_A / sqrt(1 - WGS84_E * WGS84_E * sin(lat) * sin(lat));
	const double alt = p / cos(lat) - N;
	return osg::Vec3(RAD2DEG(lat), RAD2DEG(lon), alt);
}




#if 0
#include <LASlib/lasreader.hpp>
//读取点云
osg::Node* LoadPointCloud(const string& file_path)
{
	osg::Geode *gnode = new osg::Geode;

	// 打开las文件
	LASreadOpener lasreadopener;
	lasreadopener.set_file_name(file_path.c_str());
	LASreader* lasReader = lasreadopener.open();
	size_t point_count = lasReader->header.number_of_point_records;

	int majorVersion = lasReader->header.version_major;
	int minorVersion = lasReader->header.version_minor;
	int pointDataFormat = lasReader->header.point_data_format;
	int pointAmount = lasReader->header.number_of_point_records;
	double maxX = lasReader->header.max_x;
	double minX = lasReader->header.min_x;
	double maxY = lasReader->header.max_y;
	double minY = lasReader->header.min_y;
	double maxZ = lasReader->header.max_z;
	double minZ = lasReader->header.min_z;
	double deltaX = maxX - minX;
	double deltaY = maxY - minY;
	double deltaZ = maxZ - minZ;
	double xOffset = lasReader->header.x_offset;
	double yOffset = lasReader->header.y_offset;
	double zOffset = lasReader->header.z_offset;
	double xScale = lasReader->header.x_scale_factor;
	double yScale = lasReader->header.y_scale_factor;
	double zScale = lasReader->header.z_scale_factor;


	//创建顶点数组
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;

	//创建颜色
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

	// 遍历点云
	int num = 0;

	while (lasReader->read_point())
	{
		//		if (num == 100) break;
		LASpoint& cur_point = lasReader->point;

		osgLasPoints p;
		p.x = cur_point.get_x();
		p.y = cur_point.get_y();
		p.z = cur_point.get_z();
		p.intensity = cur_point.get_intensity();
		p.gpstime = cur_point.get_gps_time();
		p.r = cur_point.get_R() / 25500.0;
		p.g = cur_point.get_G() / 25500.0;
		p.b = cur_point.get_B() / 25500.0;
		p.classification = cur_point.get_classification();


		//		coords->push_back(osg::Vec3(lasReader->point.get_x() - xOffset, lasReader->point.get_y() - yOffset, lasReader->point.get_z() - zOffset));
		coords->push_back(osg::Vec3(lasReader->point.get_x(), lasReader->point.get_y(), lasReader->point.get_z()));
		colors->push_back(osg::Vec4(cur_point.get_R() / 25500.0, cur_point.get_G() / 25500.0, cur_point.get_B() / 25500.0, cur_point.get_I() / 25500.0));

		num++;
	}

	lasReader->close();
	delete lasReader;

	//创建几何体
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

	//设置顶点数组
	geometry->setVertexArray(coords.get());
	geometry->setColorArray(colors.get());
	geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);


	//设置法向量
	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));

	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, point_count));

	gnode->addDrawable(geometry.get());

	osg::BoundingSphere bs = gnode->getBound();
	osg::Vec3d ref_center = bs.center();

	return gnode;
}

#endif

 
//读取json点云   需要给点云做偏移
osg::Node* LoadPointCloudFormJson(const string& file_path,double &ori_x, double &ori_y, double &ori_z)
{	
	osg::Geode *gnode = new osg::Geode;
 	//创建顶点数组
 	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array; 
 	//创建颜色
 	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

	//解析json文件中对应的点云信息
	QFile file(QString::fromLocal8Bit(file_path.c_str()));
	if (!file.open(QIODevice::ReadOnly))
		return nullptr;
	auto data = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(data);
	QJsonObject atData = doc.object();	
	int point_count = 0;
	bool ecef_flag = false;
	bool lla_falg = false;

	if (atData["BlocksExchange"].isObject())
	{
		auto BlocksExchange = atData["BlocksExchange"].toObject();
		//坐标系
		auto SpatialReferenceSystems = BlocksExchange["SpatialReferenceSystems"].toObject();
		QJsonArray SRS = SpatialReferenceSystems["SRS"].toArray();
		for (size_t i = 0; i < SRS.size(); i++)
		{
			QJsonObject obj = SRS.at(i).toObject();
			QString srs_def = obj["Definition"].toString();
			if (srs_def == "EPSG:4978")
			{
				ecef_flag = true;
			}
			if (srs_def == "EPSG:4326"|| srs_def == "EPSG:4490")
			{
				lla_falg = true;
			}
		}
		//数据
		auto block = BlocksExchange["Block"].toObject();
		auto TilePoints = block["TiePoints"].toObject();
		QJsonArray tiePointArray = TilePoints["TiePoint"].toArray();
		point_count = tiePointArray.size();

		if (point_count == 0)
		{
			return nullptr;
		}

		if (!ecef_flag && !lla_falg) // 非ECEF && LLA
		{
			JsonPoints	origintemp;

			QJsonObject obj = tiePointArray.at(0).toObject();
			QJsonObject Color = obj["Color"].toObject();

			origintemp.r = Color["Blue"].toString().toDouble();
			origintemp.g = Color["Green"].toString().toDouble();
			origintemp.b = Color["Red"].toString().toDouble();

			QJsonObject Position = obj["Position"].toObject();
			origintemp.x = Position["x"].toString().toDouble();
			origintemp.y = Position["y"].toString().toDouble();
			origintemp.z = Position["z"].toString().toDouble();

			double ori_x = origintemp.x;
			double ori_y = origintemp.y;
			double ori_z = origintemp.z;
			coords->push_back(osg::Vec3(0.0, 0.0, 0.0));
			colors->push_back(osg::Vec4(origintemp.r, origintemp.g, origintemp.b, 0.5));

			for (int i = 1; i < tiePointArray.size(); i++)
			{
				JsonPoints temp;

				QJsonObject obj = tiePointArray.at(i).toObject();
				QJsonObject Color = obj["Color"].toObject();

				temp.r = Color["Blue"].toString().toDouble();
				temp.g = Color["Green"].toString().toDouble();
				temp.b = Color["Red"].toString().toDouble();

				QJsonObject Position = obj["Position"].toObject();
				temp.x = Position["x"].toString().toDouble() - origintemp.x;
				temp.y = Position["y"].toString().toDouble() - origintemp.y;
				temp.z = Position["z"].toString().toDouble() - origintemp.z;

				coords->push_back(osg::Vec3(temp.x, temp.y, temp.z));
				colors->push_back(osg::Vec4(temp.r, temp.g, temp.b, 0.5));

			} 
		}else if (ecef_flag)  //ECEF
		{
			JsonPoints	origintemp;

			QJsonObject obj = tiePointArray.at(0).toObject();
			QJsonObject Color = obj["Color"].toObject();

			origintemp.r = Color["Blue"].toString().toDouble();
			origintemp.g = Color["Green"].toString().toDouble();
			origintemp.b = Color["Red"].toString().toDouble();

			QJsonObject Position = obj["Position"].toObject();
			origintemp.x = Position["x"].toString().toDouble();
			origintemp.y = Position["y"].toString().toDouble();
			origintemp.z = Position["z"].toString().toDouble();


			osg::Vec3 vec3_ori = ECEF2LLA(osg::Vec3(origintemp.x, origintemp.y, origintemp.z));

			double ori_x = vec3_ori.x();
			double ori_y = vec3_ori.y();
			double ori_z = vec3_ori.z();
			coords->push_back(osg::Vec3(0.0, 0.0, 0.0));
			colors->push_back(osg::Vec4(origintemp.r, origintemp.g, origintemp.b, 0.5));

			for (int i = 1; i < tiePointArray.size(); i++)
			{
				JsonPoints temp;

				QJsonObject obj = tiePointArray.at(i).toObject();
				QJsonObject Color = obj["Color"].toObject();

				temp.r = Color["Blue"].toString().toDouble();
				temp.g = Color["Green"].toString().toDouble();
				temp.b = Color["Red"].toString().toDouble();

				QJsonObject Position = obj["Position"].toObject();
				temp.x = Position["x"].toString().toDouble();
				temp.y = Position["y"].toString().toDouble();
				temp.z = Position["z"].toString().toDouble();

				osg::Vec3 vec3_trans = ECEF2LLA(osg::Vec3(temp.x, temp.y, temp.z));

				coords->push_back(osg::Vec3((vec3_trans.x() - ori_x) * 100000, (vec3_trans.y() - ori_y) * 100000, vec3_trans.z() - ori_z)); //10的5次方 米级精度
				colors->push_back(osg::Vec4(temp.r, temp.g, temp.b, 0.5));

			}
		}
		else if (lla_falg) //lla
		{
			JsonPoints	origintemp;

			QJsonObject obj = tiePointArray.at(0).toObject();
			QJsonObject Color = obj["Color"].toObject();

			origintemp.r = Color["Blue"].toString().toDouble();
			origintemp.g = Color["Green"].toString().toDouble();
			origintemp.b = Color["Red"].toString().toDouble();

			QJsonObject Position = obj["Position"].toObject();
			origintemp.x = Position["x"].toString().toDouble();
			origintemp.y = Position["y"].toString().toDouble();
			origintemp.z = Position["z"].toString().toDouble();

			double ori_x = origintemp.x;
			double ori_y = origintemp.y;
			double ori_z = origintemp.z;
			coords->push_back(osg::Vec3(0.0, 0.0, 0.0));
			colors->push_back(osg::Vec4(origintemp.r, origintemp.g, origintemp.b, 0.5));

			for (int i = 1; i < tiePointArray.size(); i++)
			{
				JsonPoints temp;

				QJsonObject obj = tiePointArray.at(i).toObject();
				QJsonObject Color = obj["Color"].toObject();

				temp.r = Color["Blue"].toString().toDouble();
				temp.g = Color["Green"].toString().toDouble();
				temp.b = Color["Red"].toString().toDouble();

				QJsonObject Position = obj["Position"].toObject();
				temp.x = (Position["x"].toString().toDouble() - origintemp.x) * 100000;
				temp.y = (Position["y"].toString().toDouble() - origintemp.y) * 100000;
				temp.z = Position["z"].toString().toDouble() - origintemp.z;

				coords->push_back(osg::Vec3(temp.x, temp.y, temp.z));
				colors->push_back(osg::Vec4(temp.r, temp.g, temp.b, 0.5));

			}
		}

	}

	file.close();
	//创建几何体
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

	//设置顶点数组
	geometry->setVertexArray(coords.get());
	geometry->setColorArray(colors.get());
	geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	//设置点大小
	osg::StateSet* stateSet = gnode->getOrCreateStateSet();
	osg::Point* pointSize = new osg::Point;
	pointSize->setSize(2.0);
	stateSet->setAttribute(pointSize);

	//设置法向量
	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));

	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, point_count));

	gnode->addDrawable(geometry.get());

	osg::BoundingSphere bs = gnode->getBound();
	osg::Vec3d ref_center = bs.center();

	return gnode;
}


//创建光照
osg::ref_ptr<osg::Group> createLight(osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<osg::Group> lightRoot = new osg::Group();
	lightRoot->addChild(node);
	lightRoot->setName("LightNode");

	//开启光照
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	stateset = lightRoot->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHT0, osg::StateAttribute::ON);//允许GL_LIGHT0光照
	stateset->setMode(GL_LIGHT1, osg::StateAttribute::ON);//允许GL_LIGHT1光照

														  //创建两个个Light对象
	osg::ref_ptr<osg::Light> light = new osg::Light();
	osg::ref_ptr<osg::Light> light1 = new osg::Light();
	//开启光照
	light->setLightNum(0);
	light1->setLightNum(1);//0对应GL_LIGHT0，1对应GL_LIGHT1，默认情况下为GL_LIGHT0
						   //设置方向
						   //light->setDirection(osg::Vec3(0.0f , 0.0f , -1.0f));
						   //设置位置 最后一个参数1是点光源，0是平行光
	light->setPosition(osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f));//太阳光
	light1->setPosition(osg::Vec4(100.f, 100.f, 800.f, 1.0f));


	//设置光源颜色
	light->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	light1->setAmbient(osg::Vec4(0.3f, 0.3f, 0.5f, 1.0f));
	//设置散射光的颜色
	light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	light1->setDiffuse(osg::Vec4(0.75f, 0.75f, 0.75f, 1.0f));
	//镜面反射颜色
	light->setSpecular(osg::Vec4d(0.8f, 0.8f, 0.8f, 1.0f));
	light1->setSpecular(osg::Vec4d(0.8f, 0.8f, 0.8f, 1.0f));


	//设置恒衰减指数（可使用默认参数）
	light->setConstantAttenuation(1.f);
	light1->setConstantAttenuation(1.f);
	//设置线性衰减指数
	light->setLinearAttenuation(0.f);
	light1->setLinearAttenuation(0.f);
	//light->setLinearAttenuation(0.f);
	//设置二次方衰减指数
	light->setQuadraticAttenuation(0.f);
	light1->setQuadraticAttenuation(0.f);
	//stateset->setAttribute(light,osg::StateAttribute::OFF);

	//light->setQuadraticAttenuation(0.f);

	//创建光源 
	osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
	osg::ref_ptr<osg::LightSource> lightSource1 = new osg::LightSource();
	lightSource->setLight(light.get());
	lightSource1->setLight(light1.get());
	lightRoot->addChild(lightSource.get());
	lightRoot->addChild(lightSource1.get());

	return lightRoot.get();

}


//创建坐标轴网格
osg::Node* createBase(const osg::Vec3& center, float radius)
{

	int numTilesX = 10;
	int numTilesY = 10;

	float width = 2 * radius;
	float height = 2 * radius;

	osg::Vec3 v000(center - osg::Vec3(width*0.5f, height*0.5f, 0.0f));
	osg::Vec3 dx(osg::Vec3(width / ((float)numTilesX), 0.0, 0.0f));
	osg::Vec3 dy(osg::Vec3(0.0f, height / ((float)numTilesY), 0.0f));
		
	osg::Vec3Array* coords = new osg::Vec3Array;
	int iy;
	for (iy = 0; iy <= numTilesY; ++iy)
	{
		for (int ix = 0; ix <= numTilesX; ++ix)
		{
			coords->push_back(v000 + dx*(float)ix + dy*(float)iy);
		}
	}


	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f)); // white


	osg::ref_ptr<osg::DrawElementsUShort> whitePrimitives = new osg::DrawElementsUShort(GL_QUADS);
	osg::DrawElementsUShort* primitives = whitePrimitives.get();
	int numIndicesPerRow = numTilesX + 1;
	for (iy = 0; iy < numTilesY; ++iy)
	{
		for (int ix = 0; ix < numTilesX; ++ix)
		{

			int  a = ix + (iy + 1)*numIndicesPerRow;
			int  b = ix + iy*numIndicesPerRow;
			int  c = (ix + 1) + iy*numIndicesPerRow;
			int  d = (ix + 1) + (iy + 1)*numIndicesPerRow;

			primitives->push_back(ix + (iy + 1)*numIndicesPerRow);
			primitives->push_back(ix + iy*numIndicesPerRow);
			primitives->push_back((ix + 1) + iy*numIndicesPerRow);
			primitives->push_back((ix + 1) + (iy + 1)*numIndicesPerRow);

		}

	}

	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));

	osg::Geometry* geom = new osg::Geometry;
	geom->setVertexArray(coords);

	geom->setColorArray(colors, osg::Array::BIND_PER_PRIMITIVE_SET);

	geom->setNormalArray(normals, osg::Array::BIND_OVERALL);

	geom->addPrimitiveSet(primitives);

	
	osg::Geode* geode = new osg::Geode;

	//在根节点的stateSet中禁止光照
	//使用PROTECTED 以保证这一修改不会被osgviewer覆盖
	osg::StateSet *state = geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

	osg::PolygonMode* pm = new osg::PolygonMode(
		osg::PolygonMode::FRONT_AND_BACK,
		osg::PolygonMode::LINE);
	state->setAttributeAndModes(pm);
	// 同时还设置线宽为 1。
	osg::LineWidth* lw = new osg::LineWidth(0.8);
	state->setAttribute(lw);

	geode->addDrawable(geom);
	geode->setName("grid");
	return geode;
}

//创建标签
osg::Node* createAutoScale(const osg::Vec3& position, float characterSize, const std::string& message, float minScale = 0.0, float maxScale = FLT_MAX)
{
	std::string timesFont("fonts/arial.ttf");

	osgText::Text* text = new osgText::Text;
	text->setCharacterSize(characterSize);
	text->setText(message);
	text->setFont(timesFont);
	text->setAlignment(osgText::Text::LEFT_BOTTOM_BASE_LINE);

	osg::Geode* geode = new osg::Geode;
	geode->addDrawable(text);
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	osg::AutoTransform* at = new osg::AutoTransform;
	at->addChild(geode);

	at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	at->setAutoScaleToScreen(true);
	at->setMinimumScale(minScale);
	at->setMaximumScale(maxScale);
	at->setPosition(position);

	return at;
}

//画个球
osg::Node* createSphere(osg::Vec3 center)
{
	osg::Geode* gnode = new osg::Geode;
	osg::ShapeDrawable* sd = new osg::ShapeDrawable(new osg::Sphere(center,0.1));
	sd->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));

	gnode->addDrawable(sd);
	gnode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	return gnode;
}

//画个球
osg::Node* createSphereR(osg::Vec3 center,double radius)
{
	osg::Geode* gnode = new osg::Geode;
	osg::ShapeDrawable* sd = new osg::ShapeDrawable(new osg::Sphere(center, radius));
	sd->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));

	gnode->addDrawable(sd);
	gnode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	return gnode;
}


//创建坐标轴
osg::Geode* createAxis(const osg::BoundingSphere & bs)
{
	osg::Geode* geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geometry(new osg::Geometry());

	osg::Vec3d axis_center = bs.center()-osg::Vec3d(bs.radius(), bs.radius(), bs.radius() /3);
	double axis_rad = bs.radius()/3;

	std::cout << "axis_center: " << axis_center.x() << " " << axis_center.y() << " " << axis_center.z() << " " << std::endl;

	osg::ref_ptr<osg::Vec3Array> vertices(new osg::Vec3Array());


	vertices->push_back(osg::Vec3(axis_center[0], axis_center[1], axis_center[2]));
	vertices->push_back(osg::Vec3(axis_center[0]+ axis_rad, axis_center[1], axis_center[2]));
	vertices->push_back(osg::Vec3(axis_center[0], axis_center[1], axis_center[2]));
	vertices->push_back(osg::Vec3(axis_center[0], axis_center[1]+ axis_rad, axis_center[2]));
	vertices->push_back(osg::Vec3(axis_center[0], axis_center[1], axis_center[2]));
	vertices->push_back(osg::Vec3(axis_center[0], axis_center[1], axis_center[2]+ axis_rad));
	geometry->setVertexArray(vertices.get());

	osg::ref_ptr<osg::Vec4Array> colors(new osg::Vec4Array());
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	geometry->setColorArray(colors.get(), osg::Array::BIND_PER_VERTEX);
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 6));


	geode->addDrawable(geometry.get());
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, false);

	geode->addChild(createSphere(osg::Vec3(axis_center[0] + axis_rad, axis_center[1], axis_center[2])));
	geode->addChild(createSphere(osg::Vec3(axis_center[0] , axis_center[1]+ axis_rad, axis_center[2])));
	geode->addChild(createSphere(osg::Vec3(axis_center[0] , axis_center[1], axis_center[2]+ axis_rad)));

	geode->addChild(createAutoScale(osg::Vec3(axis_center[0] + axis_rad, axis_center[1], axis_center[2]), 18.0, "X", 0, 5.0));
	geode->addChild(createAutoScale(osg::Vec3(axis_center[0], axis_center[1] + axis_rad, axis_center[2]), 18.0, "Y", 0, 5.0));
	geode->addChild(createAutoScale(osg::Vec3(axis_center[0], axis_center[1], axis_center[2] + axis_rad), 18.0, "Z", 0, 5.0));
	
	geode->setName("ASIX");
	//在根节点的stateSet中禁止光照
	//使用PROTECTED 以保证这一修改不会被osgviewer覆盖
	osg::StateSet *state = geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	// 同时还设置线宽为 1.5。
	osg::LineWidth* lw = new osg::LineWidth(1.5);
	state->setAttribute(lw);

	osg::Vec3d grid_center = bs.center() - osg::Vec3d(0, 0, bs.radius() / 3 + 1);
	geode->addChild(createBase(grid_center, bs.radius()));
	
	return geode;
}

//设置相机俯视视角
osg::ref_ptr<osg::Camera>creatLookDownEye(const osg::BoundingSphere& bs)
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;				//定义一个相机节点
	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除它的颜色和深度缓存，
																	//这意味着这个相机渲染子场景时会覆盖之前任何相机的渲染数据
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);//设置相机的参考坐标系，设置为ABSOLUTE_RF意味着相机的所有变换矩阵和观察/投影矩阵的设置
														   //都是相对于世界坐标的不会受到上一级矩阵的影响
														   //根据包围球大小，设置相机的投影矩阵
	double viewDistance = 3.5*bs.radius();
	double znear = viewDistance - bs.radius();
	double zfar = viewDistance + bs.radius();
	float top = bs.radius();
	float right = bs.radius();
	camera->setProjectionMatrixAsOrtho(-right, right, -top, top, znear, zfar);

	//根据模型的中心位置，设置相机的观察矩阵。
	osg::Vec3d upDirection(0.0, 1.0, 0.0);
	osg::Vec3d viewDirection(0.0, 0.0, 1.0);
	osg::Vec3d center = bs.center();
	osg::Vec3d eyePoint = center + viewDirection*viewDistance;
	camera->setViewMatrixAsLookAt(eyePoint, center, upDirection);

	osg::Vec3f eye, er, up;
	camera->getViewMatrixAsLookAt(eye, er, up);
	return camera.release();
}


//读取立方图
osg::ref_ptr<osg::TextureCubeMap> readCubeMap()
{
	osg::ref_ptr<osg::TextureCubeMap> cubemap = new osg::TextureCubeMap;

// 	osg::ref_ptr<osg::Image>imagePosX = osgDB::readRefImageFile("D:\\Git\\renderfarm-dayan-testView\\src\\QRClient\\UI\\skyDark\\sky_0.jpg");
// 	osg::ref_ptr<osg::Image>imageNegX = osgDB::readRefImageFile("D:\\Git\\renderfarm-dayan-testView\\src\\QRClient\\UI\\skyDark\\sky_1.jpg");
// 	osg::ref_ptr<osg::Image>imagePosY = osgDB::readRefImageFile("D:\\Git\\renderfarm-dayan-testView\\src\\QRClient\\UI\\skyDark\\sky_2.jpg");
// 	osg::ref_ptr<osg::Image>imageNegY = osgDB::readRefImageFile("D:\\Git\\renderfarm-dayan-testView\\src\\QRClient\\UI\\skyDark\\sky_3.jpg");
// 	osg::ref_ptr<osg::Image>imagePosZ = osgDB::readRefImageFile("D:\\Git\\renderfarm-dayan-testView\\src\\QRClient\\UI\\skyDark\\sky_4.jpg");
// 	osg::ref_ptr<osg::Image>imageNegZ = osgDB::readRefImageFile("D:\\Git\\renderfarm-dayan-testView\\src\\QRClient\\UI\\skyDark\\sky_5.jpg");

#define CUBEMAP_FILENAME(face) "skyDark/" #face ".jpg"

	std::string resource_path = (QCoreApplication::applicationDirPath() + "/resource/").toLocal8Bit();

	osg::ref_ptr<osg::Image>imagePosX = osgDB::readRefImageFile(resource_path + CUBEMAP_FILENAME(sky_0));
	osg::ref_ptr<osg::Image>imageNegX = osgDB::readRefImageFile(resource_path + CUBEMAP_FILENAME(sky_1));
	osg::ref_ptr<osg::Image>imagePosY = osgDB::readRefImageFile(resource_path + CUBEMAP_FILENAME(sky_2));
	osg::ref_ptr<osg::Image>imageNegY = osgDB::readRefImageFile(resource_path + CUBEMAP_FILENAME(sky_3));
	osg::ref_ptr<osg::Image>imagePosZ = osgDB::readRefImageFile(resource_path + CUBEMAP_FILENAME(sky_4));
	osg::ref_ptr<osg::Image>imageNegZ = osgDB::readRefImageFile(resource_path + CUBEMAP_FILENAME(sky_5));
	
	LOGFMTI("[osgTest] cube map[%s] ", resource_path.c_str());

	if (imagePosX.get() && imageNegX.get() && imagePosY.get() && imageNegY.get() && imagePosZ.get() && imageNegZ.get())
	{
		//设置立方图的六个面的贴图
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_X, imagePosX.get());
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X, imageNegX.get());
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y, imagePosY.get());
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y, imageNegY.get());
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z, imagePosZ.get());
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z, imageNegZ.get());

		//设置纹理环绕模式
		cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
		cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

		//设置滤波：线形和mipmap
		cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
		cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	}

	return cubemap.get();
}

//更新立方体图纹理
struct TexMatCallback : public osg::NodeCallback
{
public:

	TexMatCallback(osg::TexMat& tm) :
		_texMat(tm)
	{
		//
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			//得到模型视图矩阵并设置旋转角度
			const osg::Matrix& MV = *(cv->getModelViewMatrix());
			const osg::Matrix R = osg::Matrix::rotate(osg::DegreesToRadians(112.0f), 0.0f, 0.0f, 1.0f)*
				osg::Matrix::rotate(osg::DegreesToRadians(90.0f), 1.0f, 0.0f, 0.0f);

			osg::Quat q = MV.getRotate();
			const osg::Matrix C = osg::Matrix::rotate(q.inverse());

			//设置纹理矩阵
			_texMat.setMatrix(C*R);
		}

		traverse(node, nv);
	}

	//纹理矩阵
	osg::TexMat& _texMat;
};

//一个变换类，使天空盒绕视点旋转
class MoveEarthySkyWithEyePointTransform : public osg::Transform
{
public:
	//局部矩阵计算成世界矩阵
	virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.preMult(osg::Matrix::translate(eyePointLocal));
		}
		return true;
	}

	//世界矩阵计算为局部矩阵
	virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.postMult(osg::Matrix::translate(-eyePointLocal));
		}
		return true;
	}
};

//创建天空盒
osg::ref_ptr<osg::Node> createSkyBox()
{
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();

	//设置纹理映射方式，指定为替代方式，即纹理中的颜色代替原来的颜色
	osg::ref_ptr<osg::TexEnv> te = new osg::TexEnv;
	te->setMode(osg::TexEnv::REPLACE);
	stateset->setTextureAttributeAndModes(0, te.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	//自动生成纹理坐标，反射方式(REFLECTION_MAP)
	/*
	NORMAL_MAP　标准模式－立方图纹理
	REFLECTION_MAP　反射模式－球体纹理
	SPHERE_MAP　球体模型－球体纹理
	*/
	osg::ref_ptr<osg::TexGen> tg = new osg::TexGen;
	tg->setMode(osg::TexGen::NORMAL_MAP);
	stateset->setTextureAttributeAndModes(0, tg.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	//设置纹理矩阵
	osg::ref_ptr<osg::TexMat> tm = new osg::TexMat;
	stateset->setTextureAttribute(0, tm.get());

	//设置立方图纹理
	osg::ref_ptr<osg::TextureCubeMap> skymap = readCubeMap();
	stateset->setTextureAttributeAndModes(0, skymap.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateset->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);

	//将深度设置为远平面
	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1.0, 1.0);//远平面   
	stateset->setAttributeAndModes(depth, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	//设置渲染顺序为-1，先渲染
	stateset->setRenderBinDetails(-1, "RenderBin");

	osg::ref_ptr<osg::Drawable> drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 20));  // 天空盒的半径设置

	//把球体加入到叶节点
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->setCullingActive(false);
	geode->setStateSet(stateset.get());
	geode->addDrawable(drawable.get());

	//设置变换
	osg::ref_ptr<osg::Transform> transform = new MoveEarthySkyWithEyePointTransform();
	transform->setCullingActive(false);
	transform->addChild(geode.get());
	transform->setName("sky_transform");

	osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
	clearNode->setCullCallback(new TexMatCallback(*tm));
	clearNode->addChild(transform.get());
	clearNode->setName("sky_clearNode");

	return clearNode.get();
}








osg::ref_ptr <osg::Node>createSceneGraph_1()
{
	//创建一个用于保存几何信息的对象
	osg::ref_ptr<osg::Geometry>geom = new osg::Geometry;

	//创建四个顶点数组
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	geom->setVertexArray(v.get());
	v->push_back(osg::Vec3(-100.f, 0.f, -100.f));
	v->push_back(osg::Vec3(100.f, 0.f, -100.f));
	v->push_back(osg::Vec3(100.f, 0.f, 100.f));
	v->push_back(osg::Vec3(-100.f, 0.f, 100.f));

	//创建四种颜色的数组
	osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array;  //r,g,b,透明
	geom->setColorArray(c.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	c->push_back(osg::Vec4(1.f, 0.f, 0.f, 1.f));
	c->push_back(osg::Vec4(0.f, 1.f, 0.f, 1.f));
	c->push_back(osg::Vec4(0.f, 0.f, 1.f, 1.f));
	c->push_back(osg::Vec4(1.f, 1.f, 1.f, 1.f));

	//为唯一的法线创建一个数组
	osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array;
	geom->setNormalArray(n.get());
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	n->push_back(osg::Vec3(0.f, -1.0f, 0.f));

	//由保存的数据绘制四个顶点的多边形   可以加载多个图元数据进行显示？
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	//向Geode类添加几何体（Drawable）并返回Geode
	osg::ref_ptr<osg::Geode>geode = new osg::Geode;
	geode->addDrawable(geom.get());
	return geode.get();	
}
osg::ref_ptr <osg::Geometry>createDrawable()
{
	//创建一个用于保存几何信息的对象
	osg::ref_ptr<osg::Geometry>geom = new osg::Geometry;

	//创建四个顶点数组
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	geom->setVertexArray(v.get());
	v->push_back(osg::Vec3(-1.f, 0.f, -1.f));
	v->push_back(osg::Vec3(1.f, 0.f, -1.f));
	v->push_back(osg::Vec3(1.f, 0.f, 1.f));
	v->push_back(osg::Vec3(-1.f, 0.f, 1.f));

	v->push_back(osg::Vec3(-0.8f, -1.f, -0.8f));
	v->push_back(osg::Vec3(1.2f, -1.f, -0.8f));
	v->push_back(osg::Vec3(1.2f, -1.f, 1.2f));
	v->push_back(osg::Vec3(-0.8f, -1.f, 1.2f));

	//创建四种颜色的数组
	osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array;  //r,g,b,透明
	geom->setColorArray(c.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	c->push_back(osg::Vec4(1.f, 0.f, 0.f, 1.f));
	c->push_back(osg::Vec4(0.f, 1.f, 0.f, 1.f));
	c->push_back(osg::Vec4(0.f, 0.f, 1.f, 1.f));
	c->push_back(osg::Vec4(1.f, 1.f, 1.f, 1.f));

	c->push_back(osg::Vec4(1.f, 0.f, 0.f, 0.f));
	c->push_back(osg::Vec4(0.f, 1.f, 0.f, 0.f));
	c->push_back(osg::Vec4(0.f, 0.f, 1.f, 0.f));
	c->push_back(osg::Vec4(0.f, 0.f, 0.f, 0.f));

	//为唯一的法线创建一个数组
	osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array;
	geom->setNormalArray(n.get());
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	n->push_back(osg::Vec3(0.f, -1.0f, 0.f));

	//由保存的数据绘制四个顶点的多边形 可以加载多个图元数据进行显示？
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 8));

	return geom.get();
// 	//向Geode类添加几何体（Drawable）并返回Geode
// 	osg::ref_ptr<osg::Geode>geode = new osg::Geode;
// 	geode->addDrawable(geom.get());
// 	return geode.get();
}

osg::ref_ptr<osg::Node>createScenceGraph()
{
	//创建Group根节点
	osg::ref_ptr<osg::Group> root = new osg::Group;
	{
		//在根节点的stateSet中禁止光照
		//使用PROTECTED 以保证这一修改不会被osgviewer覆盖
		osg::StateSet *state = root->getOrCreateStateSet();
		state->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);		
	}
	//创建Geode叶节点并关联Drawable
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(createDrawable().get());
	osg::Matrix m;
	{
		//左上角，使用缺省属性渲染几何体
		osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
		m.makeTranslate(-2.f, 0.f, 2.f);
		mt->setMatrix(m);
		root->addChild(mt.get());
		mt->addChild(geode.get());		
	}
	{
		//右上角：设置着色模式为FLAT(单色)
		osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
		m.makeTranslate(2.f, 0.f, 2.f);
		mt->setMatrix(m);
		root->addChild(mt);
		mt->addChild(geode.get());
		osg::StateSet*state = mt->getOrCreateStateSet();
		osg::ShadeModel *sm = new osg::ShadeModel();
		sm->setMode(osg::ShadeModel::FLAT);
		state->setAttribute(sm);
	}
	{
		// 左下角：开启背面剔除。
		osg::ref_ptr<osg::MatrixTransform> mt =	new osg::MatrixTransform;
		m.makeTranslate(-2.f, 0.f, -2.f);
		mt->setMatrix(m);
		root->addChild(mt.get());
		mt->addChild(geode.get());
		osg::StateSet* state = mt->getOrCreateStateSet();
		osg::CullFace* cf = new osg::CullFace(); // 缺省值为 BACK 
		state->setAttributeAndModes(cf);
	}
	{
		// 右下角：设置多边形填充模式为 LINE（线框）。
		osg::ref_ptr<osg::MatrixTransform> mt =	new osg::MatrixTransform;
		m.makeTranslate(2.f, 0.f, -2.f);
		mt->setMatrix(m);
		root->addChild(mt.get());
		mt->addChild(geode.get());
		osg::StateSet* state = mt->getOrCreateStateSet();
		osg::PolygonMode* pm = new osg::PolygonMode(
				osg::PolygonMode::FRONT_AND_BACK,
				osg::PolygonMode::LINE);
		state->setAttributeAndModes(pm);
		// 同时还设置线宽为 3。
		osg::LineWidth* lw = new osg::LineWidth(3.f);
		state->setAttribute(lw);
	}

	root->addChild(createSkyBox());
	return root.get();
}


osg::Node* createHUD(osgText::Text* updateText)
{

	osg::Camera* hudCamera = new osg::Camera;
	hudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	hudCamera->setProjectionMatrixAsOrtho2D(0, 1280, 0, 1024);
	hudCamera->setViewMatrix(osg::Matrix::identity());
	hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
	hudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);

	std::string timesFont("fonts/times.ttf");

	// turn lighting off for the text and disable depth test to ensure its always ontop.
	osg::Vec3 position(150.0f, 800.0f, 0.0f);
	osg::Vec3 delta(0.0f, -60.0f, 0.0f);

// 	{
// 		osg::Geode* geode = new osg::Geode();
// 		osg::StateSet* stateset = geode->getOrCreateStateSet();
// 		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
// 		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
// 		geode->setName("simple");
// 		hudCamera->addChild(geode);
// 
// 		osgText::Text* text = new  osgText::Text;
// 		geode->addDrawable(text);
// 
// 		text->setFont(timesFont);
// 		text->setText("Picking in Head Up Displays is simple!");
// 		text->setPosition(position);
// 
// 		position += delta;
// 	}



	{ // this displays what has been selected
		osg::Geode* geode = new osg::Geode();
		osg::StateSet* stateset = geode->getOrCreateStateSet();
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		geode->setName("The text label");
		geode->addDrawable(updateText);
		hudCamera->addChild(geode);

		updateText->setCharacterSize(20.0f);
		updateText->setFont(timesFont);
		updateText->setColor(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
		updateText->setText("");
		updateText->setPosition(position);
		updateText->setDataVariance(osg::Object::DYNAMIC);

		position += delta;
	}

	return hudCamera;

}

// class to handle events with a pick
class PickHandler : public osgGA::GUIEventHandler {
public:

	PickHandler(osgText::Text* updateText) :
		_updateText(updateText) {}

	~PickHandler() {}

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

	void setLabel(const std::string& name)
	{
		if (_updateText.get()) _updateText->setText(name);
	}

protected:

	osg::ref_ptr<osgText::Text>  _updateText;
};

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	switch (ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::PUSH):
	{
		if (ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (view) pick(view, ea);
			return false;
		}

	}
	case(osgGA::GUIEventAdapter::KEYDOWN):
	{
		if (ea.getKey() == 'c')
		{
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			osg::ref_ptr<osgGA::GUIEventAdapter> event = new osgGA::GUIEventAdapter(ea);
			event->setX((ea.getXmin() + ea.getXmax())*0.5);
			event->setY((ea.getYmin() + ea.getYmax())*0.5);
			if (view) pick(view, *event);
		}
		return false;
	}
	case osgGA::GUIEventAdapter::DOUBLECLICK:
	{
		if (ea.getButtonMask() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
		{
			cout << "middle_mouse_button down" << endl;

			osgViewer::View* view = dynamic_cast<osgViewer::Viewer*>(aa.asView());
			osg::Node* sceneData = view ? view->getSceneData() : 0;
			view->getCameraManipulator();

		}
		return false;

	}

	default:
		return false;
	}
}

void PickHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea)
{
	osgUtil::LineSegmentIntersector::Intersections intersections;

	std::string gdlist = "";

//	view->getCameraManipulator()->getSideVector();

	if (view->computeIntersections(ea, intersections))
	{
		for (osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
			hitr != intersections.end();
			++hitr)
		{
			std::ostringstream os;
			os.precision(12);
			if (!hitr->nodePath.empty() && !(hitr->nodePath.back()->getName().empty()))
			{
				// the geodes are identified by name.
				os << "Object \"" << hitr->nodePath.back()->getName() << "\"" << std::endl;
				continue;
			}
			else if (hitr->drawable.valid())
			{
				const osg::NodePath& np = hitr->nodePath;
				for (int i = 0; i < np.size(); i++)
				{
					osg::Node *nd = dynamic_cast<osg::Node*>(np[i]);

					if ( nd->getName() == "First_model")
					{
						int j = 0;
						string index = to_string(i);
						os << "Object \"" << nd->getName() <<" "<< index << "\"" << std::endl;
//						osg::Vec3d world_coord = hitr->getWorldIntersectPoint() + osg::Vec3d(354007.0, 2056540.0, 0.0);  //需要添加偏移值
						osg::Vec3d world_coord = hitr->getWorldIntersectPoint() + osg::Vec3d(448776.0, 2688973.0, 0.0); 
						os << "X= " << world_coord.x() << std::endl;
						os << "Y= " << world_coord.y() << std::endl;
						os << "Z= " << world_coord.z() << std::endl;
					}
				}			
			}

			gdlist += os.str();

		}
	}
	setLabel(gdlist);
}





struct Point3D
{
	string name;
	double x;
	double y;
	double z;

	Point3D& operator = (const Point3D& _temp)
	{
		name = _temp.name;
		x = _temp.x;
		y = _temp.y;
		z = _temp.z;

		return *this;
	}
	bool operator == (const Point3D& _temp) const
	{
		return(x == _temp.x&&y == _temp.y);
	}

	bool operator != (const Point3D& _temp) const
	{
		return !(_temp == *this);
	}
	bool operator< (const Point3D& _temp) const
	{
		return (x < _temp.x || (!(x > _temp.x) && y < _temp.y));
	}


};


bool loadXYZ(const string &_filename, vector<Point3D>& _imagePoint)
{
	_imagePoint.clear();

	QFile file(QString::fromStdString(_filename));
	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream stream(&file);
		QString line = stream.readLine();
		while (!line.isNull())
		{
			Point3D imagePos;
			QStringList list = line.split(",   ");
			QListIterator<QString> li(list);
			while (li.hasNext())
			{
				imagePos.name = li.next().toStdString();
				imagePos.x = li.next().toDouble();
				imagePos.y = li.next().toDouble();
				imagePos.z = li.next().toDouble();
				li.toBack();
				//				std::cout << imagePos.name <<": " << imagePos.x <<"  "<< imagePos.y<< endl;
			}
			_imagePoint.push_back(imagePos);
			line = stream.readLine();
		}
	}
	return _imagePoint.size() > 0;
}

double angleOf(double x, double y)
{
	double  dist = sqrt(x*x + y*y);
	if (y >= 0.0)
	{
		return acos(x / dist);
	}
	else {
		return acos(-x / dist) + 0.5*CIRCLE_RADIANS;
	}
}


void createPolygon(const std::vector<Point3D>& _imageXYZ, std::vector<Point3D>&_imagePolygon)
{
	_imagePolygon.clear();
	Point3D newPoint = _imageXYZ[0];
	double  xDif, yDif, oldAngle = CIRCLE_RADIANS, newAngle, angleDif, minAngleDif;
	size_t find_start_index = 0, polygon_point_index = 0;
	for (size_t i = 0; i<_imageXYZ.size(); i++)
	{
		if (_imageXYZ[i].y > newPoint.y || _imageXYZ[i].y == newPoint.y && _imageXYZ[i].x < newPoint.x)
		{
			newPoint = _imageXYZ[i];
			find_start_index = i;
			//			std::cout << i << " " << newPoint.x << " " << newPoint.y << endl;
		}
	}
	_imagePolygon.push_back(newPoint);

#ifndef DEBUG
	std::cout << "strat: " << find_start_index << " " << newPoint.name << endl;

	std::cout << "(1,sqrt(0))  " << angleOf(1, sqrt(0)) * 360 / CIRCLE_RADIANS << endl;
	std::cout << "(1,sqrt(3))  " << angleOf(1, sqrt(3.0)) * 360 / CIRCLE_RADIANS << endl;
	std::cout << "(-1,sqrt(3))  " << angleOf(-1, sqrt(3.0)) * 360 / CIRCLE_RADIANS << endl;
	std::cout << "(-1,-sqrt(3))  " << angleOf(-1, -sqrt(3.0)) * 360 / CIRCLE_RADIANS << endl;
	std::cout << "(1,-sqrt(3))  " << angleOf(1, -sqrt(3.0)) * 360 / CIRCLE_RADIANS << endl;
	std::cout << "(1,-sqrt(0))  " << angleOf(1, -sqrt(0)) * 360 / CIRCLE_RADIANS << endl;
#endif // DEBUG

	do   //构建多边形循环   顺时针求最大角，逆时针求最小角
	{
		int  num1 = 0, num2 = 0;
		minAngleDif = CIRCLE_RADIANS;
		for (size_t i = 0; i < _imageXYZ.size(); i++)
		{
			xDif = _imageXYZ[i].x - newPoint.x;
			yDif = _imageXYZ[i].y - newPoint.y;

			double  dist = sqrt(xDif*xDif + yDif*yDif);
			//			cout <<"index: "<<i <<" - "<<dist << endl;	

			if (xDif || yDif)   //过滤掉重合的点
			{
				newAngle = angleOf(xDif, yDif);
				double new_a = newAngle * 360 / CIRCLE_RADIANS;
				angleDif = oldAngle - newAngle;

				double new_b = angleDif * 360 / CIRCLE_RADIANS;
				while (angleDif < 0.0)
					angleDif += CIRCLE_RADIANS;
				while (angleDif >= CIRCLE_RADIANS)
					angleDif -= CIRCLE_RADIANS;
				double new_c = angleDif * 360 / CIRCLE_RADIANS;

				if (angleDif < minAngleDif)
				{
					minAngleDif = angleDif;
					polygon_point_index = i;
//					double new_d = minAngleDif * 360 / CIRCLE_RADIANS;
//					std::cout << "minAngledif: " << new_d << " - " << _imageXYZ[i].name << endl;
				}
			}
		}
		//		if (polygon_point_index != find_start_index)
		{
			std::cout << "minAngledif: " << minAngleDif * 360 / CIRCLE_RADIANS << " - " << _imageXYZ[polygon_point_index].name << endl;
			_imagePolygon.push_back(_imageXYZ[polygon_point_index]);
			newPoint = _imageXYZ[polygon_point_index];
			std::cout << "   newAngle  " << newAngle * 360 / CIRCLE_RADIANS << endl;
			std::cout << "   angleDif = oldAngle - newAngle  " << angleDif * 360 / CIRCLE_RADIANS << endl;
			std::cout << "   oldAngle  " << oldAngle * 360 / CIRCLE_RADIANS << endl;
		}

		oldAngle = CIRCLE_RADIANS - minAngleDif;
		std::cout << "   --new oldAngle  " << (CIRCLE_RADIANS - minAngleDif) * 360 / CIRCLE_RADIANS << endl;
//		std::cout << newPoint.name << " - " << _imagePolygon.size() << "  oldAngle: " << oldAngle * 360 / CIRCLE_RADIANS << endl;
	} while (polygon_point_index != find_start_index);

	int i = 0;

}

osg::Node* CreateImagePos(const std::vector<Point3D>& _imageXYZ)
{
	osg::Geode *gnode = new osg::Geode;

	//创建顶点数组
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;

	//创建颜色
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4d(1.0, 0.0, 0.0, 1.0));

	for (size_t i = 0; i < _imageXYZ.size(); i++)
	{
		//coords->push_back(osg::Vec3d(_imageXYZ[i].x*1000.0, _imageXYZ[i].y*1000.0, _imageXYZ[i].z*0.00001));
		coords->push_back(osg::Vec3d(_imageXYZ[i].x, _imageXYZ[i].y, _imageXYZ[i].z*0.00000001));				
		colors->push_back(osg::Vec4d(1.0, 0.0, 0.0, 1.0));
	}

	//创建几何体
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

	//设置顶点数组
	geometry->setVertexArray(coords.get());
	geometry->setColorArray(colors);
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);


	//设置法向量
	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));

	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, _imageXYZ.size()));

	osg::StateSet* stateSet = gnode->getOrCreateStateSet();
	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

	osg::Point* pointSize = new osg::Point;
	pointSize->setSize(3.0);
	stateSet->setAttribute(pointSize);

	gnode->addDrawable(geometry.get());

	return gnode;

}


osg::Node* drawPoints(const std::vector<Point3D>& _imageXYZ)
{
	osg::Geode *gnode = new osg::Geode;

	//创建顶点数组
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;

	//创建颜色
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4d(0.0, 0.0, 1.0, 1.0));

	for (size_t i = 0; i < _imageXYZ.size(); i++)
	{
		//coords->push_back(osg::Vec3d(_imageXYZ[i].x*1000.0, _imageXYZ[i].y*1000.0, _imageXYZ[i].z*0.00001));
		coords->push_back(osg::Vec3d(_imageXYZ[i].x, _imageXYZ[i].y, _imageXYZ[i].z*0.00000001));
		colors->push_back(osg::Vec4d(0.0, 0.0, 1.0, 1.0));
	}

	//创建几何体
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

	//设置顶点数组
	geometry->setVertexArray(coords.get());
	geometry->setColorArray(colors);
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);


	//设置法向量
	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));

	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, _imageXYZ.size()));

	osg::StateSet* stateSet = gnode->getOrCreateStateSet();
	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

	osg::Point* pointSize = new osg::Point;
	pointSize->setSize(10.0);
	stateSet->setAttribute(pointSize);

	gnode->addDrawable(geometry.get());

	return gnode;

}


osg::ref_ptr <osg::Geometry>DrawPolygon(const std::vector<Point3D>& _imagePolygon)
{
	//创建一个用于保存几何信息的对象
	osg::ref_ptr<osg::Geometry>geom = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	geom->setVertexArray(v.get());

	for (size_t i = 0; i < _imagePolygon.size(); i++)
	{
		v->push_back(osg::Vec3d(_imagePolygon[i].x, _imagePolygon[i].y, 0.0));
	}

	//创建四种颜色的数组
	osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array;  //r,g,b,透明
	geom->setColorArray(c.get());
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	c->push_back(osg::Vec4(0.f, 1.f, 0.f, 1.f));

	//为唯一的法线创建一个数组
	osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array;
	geom->setNormalArray(n.get());
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	n->push_back(osg::Vec3(0.f, 1.0f, 0.f));

	//由保存的数据绘制四个顶点的多边形 可以加载多个图元数据进行显示？
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, _imagePolygon.size()));
	return geom.get();

}


struct Point2D
{
	double x;
	double y;
};

double norm(double x, double y)
{
	return sqrt(x * x + y * y);
}

void calcAveDist(const std::vector<Point3D>& _imagePos, double & _aveDist)
{
	std::map<double, string> distid;
 	for (size_t i = 0; i < _imagePos.size()-1; i++)
 	{
 		double dif_x = _imagePos[i+1].x - _imagePos[i].x;
 		double dif_y = _imagePos[i+1].y - _imagePos[i].y;
 		double len = sqrt(dif_x*dif_x + dif_y*dif_y);
		
		distid.insert(pair<double, string>(len, _imagePos[i].name)); //可能出现len 相等的情况 distid size数量会少于 _imagePos.size()-1
 	}

	if (distid.size() < 6)
	{
		return;
	}
	//去掉2个最大值和 最小值
	std::map< double, string>::iterator iterb = distid.begin();
	std::map< double, string>::iterator itere = distid.end();
	iterb++; itere--;
	iterb++; itere--;
	int num = 0;
	for (; iterb != itere; iterb++)
	{
		_aveDist += iterb->first;
		num++;
	}
	_aveDist /= distid.size()-4; 	
}


void polygonExpand(const std::vector<Point3D>& _imagePolygon, const double& expand, std::vector<Point3D>& _newPolygon)
{
	vector<Point2D> image_2d;
	for (size_t i = 0; i < _imagePolygon.size(); i++)
	{
		Point2D temp;
		temp.x = _imagePolygon[i].x;
		temp.y = _imagePolygon[i].y;
		image_2d.push_back(temp);
	}

	for (size_t i = 0; i < image_2d.size(); i++)
	{
		Point2D p = image_2d[i];
		Point2D p1 = image_2d[i == 0 ? image_2d.size() - 1 : i - 1];
		Point2D p2 = image_2d[i == image_2d.size() - 1 ? 0 : i + 1];

		double v1x = p1.x - p.x;
		double v1y = p1.y - p.y;
		double n1 = norm(v1x, v1y);
		v1x /= n1;
		v1y /= n1;

		double v2x = p2.x - p.x;
		double v2y = p2.y - p.y;
		double n2 = norm(v2x, v2y);
		v2x /= n2;
		v2y /= n2;

		double l = -expand / sqrt((1 - (v1x * v2x + v1y * v2y)) / 2);

		double vx = v1x + v2x;
		double vy = v1y + v2y;
		double n = l / norm(vx, vy);
		vx *= n;
		vy *= n;

		Point3D temp_expand;
		temp_expand.x = vx + p.x;
		temp_expand.y = vy + p.y;
		temp_expand.z = _imagePolygon[i].z*0.00000001;

		_newPolygon.push_back(temp_expand);

	}
}


struct Edge
{
	QVector2D a;
	QVector2D b;
};

void AlphaShapes(const vector<QVector2D>& _points, const double& _radius, vector<Edge>& _edges)
{
	_edges.clear();

	for (int i = 0; i <_points.size(); i++)
	{
		// k从i+1开始，减少重复计算
		for (int k = i + 1; k <_points.size(); k++)
		{
			// 跳过距离大于直径的情况
			if (_points[i].distanceToPoint(_points[k]) > 2 * _radius)
				continue;

			// 两个圆心
			QVector2D c1, c2;

			// 线段中点
			QVector2D center = 0.5*(_points[i] + _points[k]);

			// 方向向量 d = (x,y)
			QVector2D dir = _points[i] - _points[k];


			// 垂直向量 n = (a,b)  a*dir.x+b*dir.y = 0; a = -(b*dir.y/dir.x)
			QVector2D normal;
			normal.setY(5);  // 因为未知数有两个，随便给y附一个值5。 

			if (dir.x() != 0)
				normal.setX(-(normal.y()*dir.y()) / dir.x());
			else     // 如果方向平行于y轴
			{
				normal.setX(1);
				normal.setY(0);
			}
			normal.normalize();   // 法向量单位化

			float len = sqrt(_radius*_radius - (0.25*dir.length()*dir.length()));
			c1 = center + len*normal;
			c2 = center - len*normal;

			// b1、b2记录是否在圆C1、C2中找到其他点。  这里没有区分外部轮廓还是内部轮廓，后续可以跟据向量与圆心的关系优化判断，应用场景数据不会包含内轮廓；
			bool b1 = false, b2 = false;
			for (int m = 0; m < _points.size(); m++)
			{
				if (m == i || m == k)
					continue;

				if (b1 != true && _points[m].distanceToPoint(c1) < _radius)
					b1 = true;
				if (b2 != true && _points[m].distanceToPoint(c2) < _radius)
					b2 = true;

				// 如果都有内部点，不必再继续检查了
				if (b1 == true && b2 == true)
					break;
			}

			Edge edge;
			if (b1 != true || b2 != true)
			{
				edge.a = _points[i];
				edge.b = _points[k];

				_edges.push_back(edge);
			}
		}
	}

}


osg::ref_ptr <osg::Geometry>DrawPolygon2d(const std::vector<QVector2D>& _Points)
{
	//创建一个用于保存几何信息的对象
	osg::ref_ptr<osg::Geometry>geom = new osg::Geometry;

	//创建四个顶点数组
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	geom->setVertexArray(v.get());

	for (size_t i = 0; i < _Points.size(); i++)
	{
		v->push_back(osg::Vec3d(_Points[i].x() / 1000, _Points[i].y() / 1000, 0.0));
	}

	//创建颜色的数组
	osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array;  //r,g,b,透明
	geom->setColorArray(c.get());
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	c->push_back(osg::Vec4(0.f, 1.f, 0.f, 1.f));

	//为唯一的法线创建一个数组
	osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array;
	geom->setNormalArray(n.get());
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	n->push_back(osg::Vec3(0.f, 1.0f, 0.f));

	//由保存的数据绘制四个顶点的多边形 可以加载多个图元数据进行显示？
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, _Points.size()));
	return geom.get();

}


void sortPoints(std::vector<Edge>& _edges, std::vector<QVector2D>& _points)
{
	// 点序
	_points.push_back(_edges[0].a);
	_points.push_back(_edges[0].b);

	int numl = 2;
	int numr = 2;


	for (size_t i = 0; i < _edges.size(); i++)
	{
		if (_points.back() == _edges[i].a)
		{
			_points.push_back(_edges[i].b);
			i = 0;
			numl++;
		}
	}

	for (size_t i = 0; i < _edges.size(); i++)
	{
		if (_points.front() == _edges[i].a && _points[1] != _edges[i].b)
		{
			_points.insert(_points.begin(), _edges[i].b);
			i = 0;
			numr++;
		}
	}
}

void sortPoints2to3(std::vector<Edge>& _edges, std::vector<QVector2D>& _points, std::vector<Point3D>& _points3D)
{
	// 点序
	_points.push_back(_edges[0].a);
	_points.push_back(_edges[0].b);

	int numl = 2;
	int numr = 2;

	// 	for (size_t i = 0; i < _edges.size(); i++)
	// 	{
	// 		//更换线的起止点方向
	// 		QVector2D temp = _edges[i].a;
	// 
	// 		if (_points.front() == _edges[i].a)
	// 		{
	// 			Edge temp;
	// 			temp.a = _edges[i].b;
	// 			temp.b = _edges[i].a;
	// 			_edges[i] = temp;
	// 			i = 0;
	// 			numr++;
	// 		}
	// 	}

	for (size_t i = 0; i < _edges.size(); i++)
	{
		if (_points.back() == _edges[i].a)
		{
			_points.push_back(_edges[i].b);
			i = 0;
			numl++;
		}
	}

	for (size_t i = 0; i < _edges.size(); i++)
	{
		if (_points.front() == _edges[i].a && _points[1] != _edges[i].b)
		{
			_points.insert(_points.begin(), _edges[i].b);
			i = 0;
			numr++;
		}
	}

	// 	for (size_t i = 0; i < _edges.size(); i++)
	// 	{
	// 		if (_points.front() == _edges[i].b && _points.back() != _edges[i].a)
	// 		{
	// 			_points.insert(_points.begin(), _edges[i].a);
	// 			i = 0;
	// 			numr++;
	// 		}
	// 	}


	for (size_t i = 0; i<_points.size(); i++)
	{
		Point3D temp;
		temp.x = _points[i].x() / 1000;
		temp.y = _points[i].y() / 1000;
		temp.z = 0.0;
		_points3D.push_back(temp);
	}

}

//_p1,向量起点，_p2 向量终点  ，p 向量外一点
float isleft(const QVector2D & _p1, const QVector2D& _p2, QVector2D &p)
{
	QVector2D p1p2 = _p2 - _p1;
	QVector2D pp1 = _p1 - p;

	//向量差乘 ，f<0  p在p1p2左边   f>0  p在p1p2右边  f==0 p在p1p2延长线上
	float f = p1p2.x()*pp1.y() - pp1.x()*p1p2.y();

	return f;

}

/*
//计算测区的 扩展范围
//_imagePoints  测区所有影像
//_imagePolygon 测区下视镜头影像
//_radius 提取测区范围的参数半径
//_expand 外扩距离
//_newPolygon外扩新的范围
void polygonExpand2d(const std::vector<QVector2D>& _imagePoints, const std::vector<QVector2D>& _imagePolygon, const double &_radius, const double& _expand, std::vector<Point3D>& _newPolygon)
{
vector<Point2D> image_2d;

float left = 0.0;  // 外扩方向标志， left<0 ;外扩点在向量方向左侧，  left>0;外扩点在向量方向右侧

for (size_t i = 0; i < _imagePolygon.size() - 1; i++)
{
Point2D temp;
temp.x = _imagePolygon[i].x()/1000;
temp.y = _imagePolygon[i].y()/1000;
image_2d.push_back(temp);
}

//判断外扩向量方向,如果不判断，凹多边形外扩形状会产生锯齿状,根据 起实位置向量 _imagePolygon[0]_imagePolygon[1]
// 两个圆心
QVector2D c1, c2;
// 范围线线段中点
QVector2D center = 0.5*(_imagePolygon[0] + _imagePolygon[1]);
// 方向向量 d = (x,y)
QVector2D dir = _imagePolygon[1] - _imagePolygon[0];
// 垂直向量 n = (a,b)  a*dir.x+b*dir.y = 0; a = -(b*dir.y/dir.x)
QVector2D normal;
normal.setY(5);  // 因为未知数有两个，随便给y附一个值5。
if (dir.x() != 0)
normal.setX(-(normal.y()*dir.y()) / dir.x());
else     // 如果方向平行于y轴
{
normal.setX(1);
normal.setY(0);
}
normal.normalize();   // 法向量单位化
float len = sqrt(_radius*_radius - (0.25*dir.length()*dir.length()));
c1 = center + len*normal;
c2 = center - len*normal;

// b1、b2记录是否在圆C1、C2中找到其他点。
bool b1 = false, b2 = false;
for (int m = 0; m < _imagePoints.size(); m++)
{
if (_imagePolygon[0] == _imagePoints[m] || _imagePolygon[1] == _imagePoints[m])
continue;

if (b1 != true && _imagePoints[m].distanceToPoint(c1) < _radius)
b1 = true;
if (b2 != true && _imagePoints[m].distanceToPoint(c2) < _radius)
b2 = true;

if (b1 == true)
{
//判断 圆心c2方向
left = isleft(_imagePolygon[0], _imagePolygon[1], c2) < 0 ? -1.0 : 1.0;
break; //不需要在继续判断
}

if (b2 == true)
{
//判断 圆心c1方向
left = isleft(_imagePolygon[0], _imagePolygon[1], c1) < 0 ? -1.0 : 1.0;
break; //不需要在继续判断
}
}//left判断完毕

for (size_t i = 0; i < image_2d.size(); i++)
{

if (i == 24)
{
int a = 0;
}

Point2D p = image_2d[i];
Point2D p1 = image_2d[i == 0 ? image_2d.size() - 1 : i - 1];
Point2D p2 = image_2d[i == image_2d.size() - 1 ? 0 : i + 1];

//向量pp1 及归一化
double v1x = p1.x - p.x;
double v1y = p1.y - p.y;
double n1 = norm(v1x, v1y);
v1x /= n1;
v1y /= n1;

//向量pp2 及归一化
double v2x = p2.x - p.x;
double v2y = p2.y - p.y;
double n2 = norm(v2x, v2y);
v2x /= n2;
v2y /= n2;

// 计算到外扩点的位置
double l = -_expand / sqrt((1 - (v1x * v2x + v1y * v2y)) / 2);

double vx = v1x + v2x;
double vy = v1y + v2y;
double n = l / norm(vx, vy);
vx *= n;
vy *= n;

// 外扩点位置
double vpx = vx + p.x;
double vpy = vy + p.y;


// 如果left <0 ,leftv2 也会小于0  ，如果leftv2大于0 ，则重新计算更新外扩点位置(外扩点在线段的另一个方向)
float leftv1 = 0.0, leftv2 = 0.0;  //如果leftv1 与leftv2 符号一定是不一致的
//外扩点到p点的向量(vex,vey)
double vex = p.x - vpx;
double vey = p.y - vpy;
//		leftv1 = v1x*vey - vex*v1y;
leftv2 = v2x*vey - vex*v2y;

if ((left < 0 && leftv2>0) || (left > 0 && leftv2 < 0))
{
//向量pp1 及归一化
v1x = -(p1.x - p.x);
v1y = -(p1.y - p.y);
n1 = norm(v1x, v1y);
v1x /= n1;
v1y /= n1;

//向量pp2 及归一化
v2x = -(p2.x - p.x);
v2y = -(p2.y - p.y);
n2 = norm(v2x, v2y);
v2x /= n2;
v2y /= n2;

// 计算p到外扩点的距离
l = -_expand / sqrt((1 - (v1x * v2x + v1y * v2y)) / 2);

vx = v1x + v2x;
vy = v1y + v2y;
n = l / norm(vx, vy);
vx *= n;
vy *= n;


}

Point3D temp_expand;
temp_expand.x = vx + p.x;
temp_expand.y = vy + p.y;
temp_expand.z = 0.0;

_newPolygon.push_back(temp_expand);

}

}

*/


//计算测区的 扩展范围
//_imagePoints  测区所有影像
//_imagePolygon 测区下视镜头影像
//_radius 提取测区范围的参数半径
//_expand 外扩距离
//_newPolygon外扩新的范围
void polygonExpand2d(const std::vector<QVector2D>& _imagePoints, const std::vector<QVector2D>& _imagePolygon, const double &_radius, const double& _expand, std::vector<Point3D>& _newPolygon)
{
	//	vector<Point2D> image_2d;
	float left = 0.0;  // 外扩方向标志， left<0 ;外扩点在向量方向左侧，  left>0;外扩点在向量方向右侧
	QVector2D c1, c2;
	// 范围线线段中点
	QVector2D center = 0.5*(_imagePolygon[0] + _imagePolygon[1]);
	// 方向向量 d = (x,y)
	QVector2D dir = _imagePolygon[1] - _imagePolygon[0];
	// 垂直向量 n = (a,b)  a*dir.x+b*dir.y = 0; a = -(b*dir.y/dir.x)
	QVector2D normal;
	normal.setY(5);  // 因为未知数有两个，随便给y附一个值5。 
	if (dir.x() != 0)
		normal.setX(-(normal.y()*dir.y()) / dir.x());
	else     // 如果方向平行于y轴
	{
		normal.setX(1);
		normal.setY(0);
	}
	normal.normalize();   // 法向量单位化
	float len = sqrt(_radius*_radius - (0.25*dir.length()*dir.length()));
	c1 = center + len*normal;
	c2 = center - len*normal;

	// b1、b2记录是否在圆C1、C2中找到其他点。 
	bool b1 = false, b2 = false;
	for (int m = 0; m < _imagePoints.size(); m++)
	{
		if (_imagePolygon[0] == _imagePoints[m] || _imagePolygon[1] == _imagePoints[m])
			continue;

		if (b1 != true && _imagePoints[m].distanceToPoint(c1) < _radius)
			b1 = true;
		if (b2 != true && _imagePoints[m].distanceToPoint(c2) < _radius)
			b2 = true;

		if (b1 == true)
		{
			//判断 圆心c2方向
			left = isleft(_imagePolygon[0], _imagePolygon[1], c2) < 0 ? -1.0 : 1.0;
			break; //不需要在继续判断
		}

		if (b2 == true)
		{
			//判断 圆心c1方向 
			left = isleft(_imagePolygon[0], _imagePolygon[1], c1) < 0 ? -1.0 : 1.0;
			break; //不需要在继续判断
		}
	}//left判断完毕

	for (size_t i = 0; i < _imagePolygon.size() - 1; i++)
	{
		if (i == 2)
		{
			int a = 0;
		}
		QVector2D p = _imagePolygon[i];
		QVector2D p1 = _imagePolygon[i == 0 ? _imagePolygon.size() - 2 : i - 1];   //_imagePolygon 起点、止点相同
		QVector2D p2 = _imagePolygon[i == _imagePolygon.size() - 1 ? 0 : i + 1];

		//向量pp1 及归一化
		double v1x = p1.x() - p.x();
		double v1y = p1.y() - p.y();
		double n1 = norm(v1x, v1y);
		v1x /= n1;
		v1y /= n1;

		//向量pp2 及归一化
		double v2x = p2.x() - p.x();
		double v2y = p2.y() - p.y();
		double n2 = norm(v2x, v2y);
		v2x /= n2;
		v2y /= n2;

		// 计算到外扩点的位置
		double l = -_expand / sqrt((1 - (v1x * v2x + v1y * v2y)) / 2);
		double vx = v1x + v2x;
		double vy = v1y + v2y;
		double n = l / norm(vx, vy);
		if (vx == 0.0 && vy == 0.0)
		{
			//方向向量
			QVector2D dir = p2 - p1;   // p ,p1,p2 在一条直线上 ∠p1 p p2 夹角为180°
			//垂直向量
			QVector2D normal;
			normal.setY(5);  // 因为未知数有两个，随便给y附一个值5。 

			if (dir.x() != 0)
				normal.setX(-(normal.y()*dir.y()) / dir.x());
			else     // 如果方向平行于y轴
			{
				normal.setX(1);
				normal.setY(0);
			}
			normal.normalize();   // 法向量单位化
			n = l / normal.length();
			if (!b1) // 如果在圆心c1在 外侧
			{
				vx = -normal.x();
				vy = -normal.y();
			}
			else
			{
				vx = normal.x();
				vy = normal.y();
			}
		}
		vx *= n;
		vy *= n;

		// 外扩点位置
		double vpx = vx + p.x();
		double vpy = vy + p.y();

		// 如果left <0 ,leftv2 也会小于0  ，如果leftv2大于0 ，则重新计算更新外扩点位置(外扩点在线段的另一个方向)
		float leftv1 = 0.0, leftv2 = 0.0;  //如果leftv1 与leftv2 符号一定是不一致的 
										   //外扩点到p点的向量(vex,vey)
		double vex = p.x() - vpx;
		double vey = p.y() - vpy;
		leftv1 = v1x*vey - vex*v1y;
		leftv2 = v2x*vey - vex*v2y;

		if ((left < 0 && leftv2>0) /*|| (left > 0 && leftv2 < 0)*/)
		{
			//向量pp1 及归一化
			v1x = -(p1.x() - p.x());
			v1y = -(p1.y() - p.y());
			n1 = norm(v1x, v1y);
			v1x /= n1;
			v1y /= n1;

			//向量pp2 及归一化
			v2x = -(p2.x() - p.x());
			v2y = -(p2.y() - p.y());
			n2 = norm(v2x, v2y);
			v2x /= n2;
			v2y /= n2;

			// 计算p到外扩点的距离
			l = -_expand / sqrt((1 - (v1x * v2x + v1y * v2y)) / 2);

			vx = v1x + v2x;
			vy = v1y + v2y;
			n = l / norm(vx, vy);

			if (vx == 0.0 && vy == 0.0)
			{
				//方向向量
				QVector2D dir = p2 - p1;

				//垂直向量
				QVector2D normal;
				normal.setY(5);  // 因为未知数有两个，随便给y附一个值5。 

				if (dir.x() != 0)
					normal.setX(-(normal.y()*dir.y()) / dir.x());
				else     // 如果方向平行于y轴
				{
					normal.setX(1);
					normal.setY(0);
				}
				normal.normalize();   // 法向量单位化
				n = l / normal.length();
				if (!b1) // 如果在圆心c1在 外侧
				{
					vx = normal.x();
					vy = normal.y();
				}
				else
				{
					vx = -normal.x();
					vy = -normal.y();
				}
			}
			vx *= n;
			vy *= n;
		}
		Point3D temp_expand;
		temp_expand.x = (vx + p.x()) / 1000;
		temp_expand.y = (vy + p.y()) / 1000;
		temp_expand.z = 0.0;
		_newPolygon.push_back(temp_expand);
	}

}

//查找最近点
void calcRadius(const std::vector<Point3D>& _imagePos, const Point3D & _currimage, std::vector<Point3D>& _nearPoints,double& _radius)
{
	_nearPoints.clear();
	std::map<double, Point3D> curlen;
	short num = 0;
	for (size_t i = 0; i < _imagePos.size(); i++)
	{
		double dif_x = _imagePos[i].x - _currimage.x;
		double dif_y = _imagePos[i].y - _currimage.y;
		double len = sqrt(dif_x*dif_x +dif_y*dif_y);
		if (curlen.count(len) != 0)
		{
			len += 0.00001;
		}
		curlen.insert(std::pair<double, Point3D>(len, _imagePos[i]));  //可能出现len距离完全一致的数据 map 无法加载
	}
	for (auto it = curlen.begin(); it != curlen.end(); it++)
	{
		_nearPoints.push_back(it->second);		
		if (num<5)
		{
			_radius += it->first;
		}
		num++;
		if (30 == num)
		{
			break;
		}
	}
	_radius /= 5;
}

//查找最近点
void calcRadiusFilter(const std::vector<Point3D>& _imagePos, const std::vector<Point3D>& _EdgePos, const Point3D & _currimage, std::vector<Point3D>& _nearPoints, double& _radius)
{
	_nearPoints.clear();
	std::map<double, Point3D> curlen;
	short num = 0;
	for (size_t i = 0; i < _imagePos.size(); i++)
	{
		double dif_x = _imagePos[i].x - _currimage.x;
		double dif_y = _imagePos[i].y - _currimage.y;
		double len = sqrt(dif_x*dif_x + dif_y*dif_y);
		if (curlen.count(len) != 0)
		{
			len += 0.00001;
		}
		curlen.insert(std::pair<double, Point3D>(len, _imagePos[i]));  //可能出现len距离完全一致的数据 map 
	}
	for (auto it = curlen.begin(); it != curlen.end(); it++)
	{
		//如果是已存在的边界点，且不是第一个起点，则过滤掉
		if (std::find(_EdgePos.begin(), _EdgePos.end(), it->second) != _EdgePos.end()&& it->second != _EdgePos[0])
		{
			continue;
		}
		_nearPoints.push_back(it->second);
		if (num < 5)
		{
			_radius += it->first;
		}
		num++;
		if (20 == num)
		{
			break;
		}
	}
//	std::reverse(_nearPoints.begin(), _nearPoints.end());  //尝试从最远值开始做后续的 计算，还是无法避免某些点计算异常的问题；
	_radius /= 5;
	int i = 0;
}

double pointsDistance2D(const Point3D & _point1, const Point3D & _point2)
{
	double dif_x = _point1.x - _point2.x;
	double dif_y = _point1.y - _point2.y;
	return sqrt(dif_x*dif_x + dif_y*dif_y);
}

//过滤极值点（极近值和极远值）
void filterPos(const std::vector<Point3D>& _imagePos, const double& _aveDist, std::vector<Point3D>& _filterImagePos)
{
	std::map<string, double> distimage;
	for (size_t i = 0; i < _imagePos.size() - 1; i++)
	{
		double dif_x = _imagePos[i + 1].x - _imagePos[i].x;
		double dif_y = _imagePos[i + 1].y - _imagePos[i].y;
		double len = sqrt(dif_x*dif_x + dif_y*dif_y);
		distimage.insert(pair<string, double>(_imagePos[i].name, len)); //可能出现len 相等的情况 distid size数量会少于 _imagePos.size()-1
	}
	for (size_t i = 0;i<_imagePos.size();i++)
	{
		if ((distimage[_imagePos[i].name] < 0.3 * _aveDist) || (distimage[_imagePos[i].name] > 2 * _aveDist))
		{
			continue;
		}
		_filterImagePos.push_back(_imagePos[i]);
	}
}

//计算空三区域范围
void calcAtArea(std::vector<Point3D>& _imagePos, std::map<string,string>& _mapPointName, std::vector<Edge>& _edges)
{
	_mapPointName.clear();
	_edges.clear();
	//查找起点位置  

	std::vector<Point3D> filterImagePos;
	std::vector<Point3D> EdgePoints;
	double  aveDist;  //平均距离用来过滤查找最近点的可能出现的极近值
	calcAveDist(_imagePos, aveDist);
	filterPos(_imagePos, aveDist, filterImagePos);
	std::map<string, bool> isEdgePoint;
	if (_imagePos.empty())
	{
		cout << "imagePos data is empty" << endl;
		return;
	}

// 	vector<QVector2D> _imagePos2D;   
// 	for (size_t i = 0; i < _imagePos.size(); i++)
// 	{
// 		QVector2D temp(_imagePos[i].x * 1000, _imagePos[i].y * 1000);
// 		_imagePos2D.push_back(temp);
// 	}

	Point3D statr_point = _imagePos[0];   //_imagePos会重新计算，_imagePos[0]，这个起点位置可以排除内圈数据 ，一定是外环数据
	Point3D newpoint;  //新的边界点
	Point3D prePoint;  //上一个边界点
	int find_start_index = 0;
	double radius;
	float  f_radius;
	vector<Point3D> nearPoints;

	string firstName;
	string secondName;
	for (size_t i = 1; i < _imagePos.size(); i++)
	{
		if (_imagePos[i].y > statr_point.y || _imagePos[i].y == statr_point.y && _imagePos[i].x < statr_point.x)
		{
			statr_point = _imagePos[i];
			find_start_index = i;
			//std::cout << i << " " << newPoint.x << " " << newPoint.y << endl;
		}
	}
	//向量整体排序 将find_start_index 平移到首位，
	if (statr_point != _imagePos[0])
	{
		for (size_t i = 0; i < find_start_index; i++)
		{
			_imagePos.push_back(_imagePos[0]);
			_imagePos.erase(_imagePos.begin());
		}
	}
// 	int i = 0;
	newpoint = _imagePos[0];
	EdgePoints.push_back(newpoint);
//	for (size_t i = 0;i<_imagePos.size();i++)
	do
	{		
		firstName = newpoint.name;
		cout << firstName << endl;

		if (firstName == "C1C00039" )  //Block1_Sortie1_B00287.JPG   C1C00017
		{
			int c = 0;
		}

		calcRadius(_imagePos, newpoint, nearPoints, radius);
//		calcRadiusFilter(_imagePos, EdgePoints, newpoint, nearPoints, radius);

		if (radius < aveDist)
			radius = aveDist * aveDist / radius;

		f_radius = radius * 1000;
		
		for (size_t k = 0; k < nearPoints.size(); k++)
		{

			if (pointsDistance2D(newpoint, nearPoints[k]) == 0 || pointsDistance2D(newpoint, nearPoints[k]) > 2 * radius )
			{
				continue;
			}
			// 两个圆心
			QVector2D c1, c2;
			// 线段中点
			QVector2D center;
			center.setX((newpoint.x + nearPoints[k].x) * 1000 / 2);
			center.setY((newpoint.y + nearPoints[k].y) * 1000 / 2);
			// 方向向量 d = (x,y)
			QVector2D dir;
			dir.setX((newpoint.x - nearPoints[k].x) * 1000);
			dir.setY((newpoint.y - nearPoints[k].y) * 1000);

			// 垂直向量 n = (a,b)  a*dir.x+b*dir.y = 0; a = -(b*dir.y/dir.x)
			QVector2D normal;
			normal.setY(5);  // 因为未知数有两个，随便给y附一个值5。 

			if (dir.x() != 0)
				normal.setX(-(normal.y()*dir.y()) / dir.x());
			else     // 如果方向平行于y轴
			{
				normal.setX(1);
				normal.setY(0);
			}
			normal.normalize();   // 法向量单位化

			float len = sqrt(f_radius*f_radius - (0.25*dir.length()*dir.length()));
//			float len = sqrt(dir.length()*dir.length()*1.2 - (0.25*dir.length()*dir.length()));
			c1 = center + len*normal;
			c2 = center - len*normal;

			// b1、b2记录是否在圆C1、C2中找到其他点。 
			bool b1 = false, b2 = false;
			QVector2D curr_point;
			int curr_index = 0;
			std::set<QVector2D, string> set_point_name;

			float disc1 = 9999.999999;
			float disc2 = 9999.999999;
			for (int m = 0; m < _imagePos.size(); m++)
			{		
				if( (pointsDistance2D(newpoint, _imagePos[m]) == 0) || (pointsDistance2D(nearPoints[k], _imagePos[m]) == 0 ))
				{
					continue;
				}				
				curr_point.setX(_imagePos[m].x * 1000);
				curr_point.setY(_imagePos[m].y * 1000);
				curr_index = m;
				disc1 = curr_point.distanceToPoint(c1);
				disc2 = curr_point.distanceToPoint(c2);
				if (b1 != true && disc1 < f_radius)
				{
					b1 = true;
				}
				if (b2 != true && disc2 < f_radius)
				{
					b2 = true;
				}
				// 如果都有内部点，不必再继续检查了
				if (b1 == true && b2 == true)
				{
					break;
				}					
			}	
			Edge edge;
			if (b1 != true || b2 != true)
			{
				QVector2D a(newpoint.x * 1000, newpoint.y * 1000);
				QVector2D b(nearPoints[k].x * 1000, nearPoints[k].y * 1000);
				edge.a = a;
				edge.b = b;
				secondName = nearPoints[k].name;
				if (secondName == prePoint.name)
				{
					continue;
				}

				if (secondName != _imagePos[0].name && _mapPointName.count(secondName) != 0)
				{
					continue;
				}
				_edges.push_back(edge);

				prePoint = newpoint;
				newpoint = nearPoints[k];
				EdgePoints.push_back(newpoint);
				if (_mapPointName.size() > 0)
				{
					map<string, string>::iterator iter = _mapPointName.begin();
					bool input = true;
					while (iter != _mapPointName.end()) 
					{
						if (iter->first != secondName&&iter->second != firstName)
						{
							input = false;							
						}
						if (!input)
						{
							_mapPointName.insert(pair<string, string>(firstName, secondName));
						}						
						iter++;
					}
				}
				else
				{
					_mapPointName.insert(pair<string, string>(firstName, secondName));;
				}

 				break;
			}

		}

	} while (newpoint != _imagePos[0]);
	

}

//计算空三区域范围
void calcAtAreafilter(std::vector<Point3D>& _imagePos, std::map<string, string>& _mapPointName, std::vector<Edge>& _edges)
{
	_mapPointName.clear();
	_edges.clear();
	//查找起点位置  

	std::vector<Point3D> filterImagePos;
	double  aveDist;  //平均距离用来过滤查找最近点的可能出现的极近值
	calcAveDist(_imagePos, aveDist);
	filterPos(_imagePos, aveDist, filterImagePos);
	std::map<string, bool> isEdgePoint;
	if (filterImagePos.empty())
	{
		cout << "imagePos data is empty" << endl;
		return;
	}



	vector<QVector2D> _imagePos2D;   //这个起点位置可以排除内圈数据 ，一定是外环数据
	for (size_t i = 0; i < filterImagePos.size(); i++)
	{
		QVector2D temp(filterImagePos[i].x * 1000, filterImagePos[i].y * 1000);
		_imagePos2D.push_back(temp);
	}
	Point3D statr_point = filterImagePos[0];
	Point3D newpoint;  //新的边界点
	Point3D prePoint;  //上一个边界点
	int find_start_index = 0;
	double radius;
	float  f_radius;
	vector<Point3D> nearPoints;

	string firstName;
	string secondName;
	for (size_t i = 1; i < filterImagePos.size(); i++)
	{
		if (filterImagePos[i].y > statr_point.y || filterImagePos[i].y == statr_point.y && filterImagePos[i].x < statr_point.x)
		{
			statr_point = filterImagePos[i];
			find_start_index = i;
			//std::cout << i << " " << newPoint.x << " " << newPoint.y << endl;
		}
	}
	//向量整体排序 将find_start_index 平移到首位，
	if (statr_point != filterImagePos[0])
	{
		for (size_t i = 0; i < find_start_index; i++)
		{
			filterImagePos.push_back(filterImagePos[0]);
			filterImagePos.erase(filterImagePos.begin());
		}
	}
	// 	int i = 0;
	newpoint = filterImagePos[0];

	//	for (size_t i = 0;i<_imagePos.size();i++)
	do
	{
		firstName = newpoint.name;
		cout << firstName << endl;

		if (firstName == "D0059")  //Block1_Sortie1_B00287.JPG   C1C00017
		{
			int c = 0;
		}

		calcRadius(filterImagePos, newpoint, nearPoints, radius);

// 		if (radius < aveDist)
// 			radius = aveDist;

		f_radius = radius * 1000;

		for (size_t k = 0; k < nearPoints.size(); k++)
		{

			if (pointsDistance2D(newpoint, nearPoints[k]) == 0 || pointsDistance2D(newpoint, nearPoints[k]) > 2 * radius)
			{
				continue;
			}
			// 两个圆心
			QVector2D c1, c2;
			// 线段中点
			QVector2D center;
			center.setX((newpoint.x + nearPoints[k].x) * 1000 / 2);
			center.setY((newpoint.y + nearPoints[k].y) * 1000 / 2);
			// 方向向量 d = (x,y)
			QVector2D dir;
			dir.setX((newpoint.x - nearPoints[k].x) * 1000);
			dir.setY((newpoint.y - nearPoints[k].y) * 1000);

			// 垂直向量 n = (a,b)  a*dir.x+b*dir.y = 0; a = -(b*dir.y/dir.x)
			QVector2D normal;
			normal.setY(5);  // 因为未知数有两个，随便给y附一个值5。 

			if (dir.x() != 0)
				normal.setX(-(normal.y()*dir.y()) / dir.x());
			else     // 如果方向平行于y轴
			{
				normal.setX(1);
				normal.setY(0);
			}
			normal.normalize();   // 法向量单位化

			float len = sqrt(f_radius*f_radius - (0.25*dir.length()*dir.length()));
			c1 = center + len*normal;
			c2 = center - len*normal;

			// b1、b2记录是否在圆C1、C2中找到其他点。 
			bool b1 = false, b2 = false;
			QVector2D curr_point;
			int curr_index = 0;
			std::set<QVector2D, string> set_point_name;

			float disc1 = 9999.999999;
			float disc2 = 9999.999999;
			for (int m = 0; m < filterImagePos.size(); m++)
			{
				if ((pointsDistance2D(newpoint, filterImagePos[m]) == 0) || (pointsDistance2D(nearPoints[k], filterImagePos[m]) == 0))
				{
					continue;
				}
				curr_point.setX(filterImagePos[m].x * 1000);
				curr_point.setY(filterImagePos[m].y * 1000);
				curr_index = m;
				disc1 = curr_point.distanceToPoint(c1);
				disc2 = curr_point.distanceToPoint(c2);
				if (b1 != true && disc1 < f_radius)
				{
					b1 = true;
					//					cout << _imagePos[m].name << endl;
				}
				if (b2 != true && disc2 < f_radius)
				{
					b2 = true;
					//					cout << _imagePos[m].name << endl;
				}

				// 如果都有内部点，不必再继续检查了
				if (b1 == true && b2 == true)
					break;
			}

			Edge edge;
			if (b1 != true || b2 != true)
			{
				QVector2D a(newpoint.x * 1000, newpoint.y * 1000);
				QVector2D b(nearPoints[k].x * 1000, nearPoints[k].y * 1000);
				edge.a = a;
				edge.b = b;
				secondName = nearPoints[k].name;
				if (secondName == prePoint.name)
				{
					continue;
				}

				if (secondName != filterImagePos[0].name && _mapPointName.count(secondName) != 0)
				{
					continue;
				}
				_edges.push_back(edge);

				prePoint = newpoint;
				newpoint = nearPoints[k];
				if (_mapPointName.size() > 0)
				{
					map<string, string>::iterator iter = _mapPointName.begin();
					bool input = true;
					while (iter != _mapPointName.end())
					{
						if (iter->first != secondName&&iter->second != firstName)
						{
							input = false;
						}
						if (!input)
						{
							_mapPointName.insert(pair<string, string>(firstName, secondName));
						}
						iter++;
					}
				}
				else
				{
					_mapPointName.insert(pair<string, string>(firstName, secondName));;
				}

				break;
			}

		}

	} while (newpoint != filterImagePos[0]);


}


osg::ref_ptr <osg::Geometry>DrawEdge(const Edge& _edges)
{
	//创建一个用于保存几何信息的对象
	osg::ref_ptr<osg::Geometry>geom = new osg::Geometry;

	//创建顶点数组
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	geom->setVertexArray(v.get());
	
	v->push_back(osg::Vec3d(_edges.a.x() / 1000, _edges.a.y() / 1000, 0.0));		
	v->push_back(osg::Vec3d(_edges.b.x() / 1000, _edges.b.y() / 1000, 0.0));


	//创建颜色的数组
	osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array;  //r,g,b,透明
	geom->setColorArray(c.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	c->push_back(osg::Vec4(0.f, 1.f, 0.f, 1.f));
	c->push_back(osg::Vec4(0.f, 0.f, 1.f, 1.f));

	//为唯一的法线创建一个数组
	osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array;
	geom->setNormalArray(n.get());
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	n->push_back(osg::Vec3(0.f, 1.0f, 0.f));


	
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 2));
	return geom.get();

}

osg::Node* DrawLines(const std::vector<Edge>& _edges)
{
	osg::Geode *gnode = new osg::Geode;

	for (size_t i = 0; i < _edges.size(); i++)
	{
		gnode->addDrawable(DrawEdge(_edges[i]));
	}

	osg::StateSet *state = gnode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

	// 同时还设置线宽为 3.5。
	osg::LineWidth* lw = new osg::LineWidth(3.5);
	state->setAttribute(lw);
	return gnode;
}

osg::Drawable *createTriangulate(const std::vector<Point3D>& imagePos)
{
	//创建顶点数组
	osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array();
	//添加顶点数据
	for (size_t i = 0; i < imagePos.size(); i++)
	{
		osg::Vec3 vertex(imagePos[i].x*1000.0, imagePos[i].y*1000.0, 0.0);
		coords->push_back(vertex);
	}

	//创建颜色数组
	osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array();
	//添加颜色数据 
	for (unsigned int i = 0; i < imagePos.size(); i++)
	{
/*		if (i % 2 == 1)
		{
			color->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
		}
		else
		{
			color->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
		}*/
		color->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	}

	//创建Delaunay三角网对象
	osg::ref_ptr<osgUtil::DelaunayTriangulator> dt = new osgUtil::DelaunayTriangulator(coords.get());
	//生成三角网
	dt->triangulate();

	//创建几何体
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
	//设置顶点数组
	geometry->setVertexArray(coords.get());

	//设置颜色数组
	geometry->setColorArray(color.get());
	//设置颜色的绑定方式为单个顶点
	geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	//添加到绘图基元
	geometry->addPrimitiveSet(dt->getTriangles());

	return geometry.release();
}
