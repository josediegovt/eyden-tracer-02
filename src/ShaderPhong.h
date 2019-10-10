#pragma once

#include "ShaderFlat.h"

class CScene;

class CShaderPhong : public CShaderFlat
{
public:
	/**
	 * @brief Constructor
	 * @param scene Reference to the scene
	 * @param color The color of the object
	 * @param ka The ambient coefficient
	 * @param kd The diffuse reflection coefficients
	 * @param ks The specular refelection coefficients
	 * @param ke The shininess exponent
	 */
	CShaderPhong(CScene& scene, Vec3f color, float ka, float kd, float ks, float ke)
		: CShaderFlat(color)
		, m_scene(scene)
		, m_ka(ka)
		, m_kd(kd)
		, m_ks(ks)
		, m_ke(ke)
	{}
	virtual ~CShaderPhong(void) = default;

	virtual Vec3f Shade(const Ray& ray) const override{
		// --- PUT YOUR CODE HERE ---
		int size = m_scene.m_vpLights.size();
		Ray light_ray, incident_ray;
		Vec3f Ca, Cd, Cs;
		Vec3f difference = 0;
		Vec3f specular = 0;
		int i = 0;

		while(i != size){
			light_ray.org = ray.org + (ray.t * ray.dir);
			light_ray.t = std::numeric_limits<float>::infinity();
			incident_ray.org = ray.org + (ray.t * ray.dir);
			incident_ray.t = std::numeric_limits<float>::infinity();
			std::optional<Vec3f> lightRadianceLR = m_scene.m_vpLights[i]->Illuminate(light_ray);
			std::optional<Vec3f> lightRadianceIR = m_scene.m_vpLights[i]->Illuminate(incident_ray);
			if(!m_scene.Occluded(light_ray)){
				if(lightRadianceLR){
					double angle = max(light_ray.dir.dot(ray.hit->GetNormal(ray)), 0.0f);
					difference += *lightRadianceLR  * angle;
				}
				if(lightRadianceIR){
					Vec3f reflected_direction = incident_ray.dir - (2 * (incident_ray.dir.dot(ray.hit->GetNormal(ray))) * ray.hit->GetNormal(ray));
					double angle = max(ray.dir.dot(reflected_direction), 0.0f);
					double power = pow(angle, m_ke);
					specular += *lightRadianceIR  * power;
				}
			}
			i++;
		}

		Ca = m_ka * CShaderFlat::Shade(ray);
		Cd = m_kd * CShaderFlat::Shade(ray).mul(difference);
		Cs = m_ks * RGB(1,1,1).mul(specular);

		Vec3f result = Ca + Cd + Cs;
		return result;
	}


private:
	CScene& m_scene;
	float 	m_ka;    ///< ambient coefficient
	float 	m_kd;    ///< diffuse reflection coefficients
	float 	m_ks;    ///< specular refelection coefficients
	float 	m_ke;    ///< shininess exponent
};
