#include "MyVulkanEngine.h"

void initRenderObject(RenderObject *pObj, PipelineCreateInfo createInfo, mat4 *pMView)
{
	pObj->pipelineCreateInfo = createInfo;
	identity4(pObj->mModel);
	pObj->pMView = pMView;
	identity4(pObj->mProj);
}

void motion(RenderObject *pObj, mat4 rotX, mat4 rotY, mat4 rotZ, mat4 transT)
{
	mat4 tmp;

	dup4(tmp, pObj->mModel);
	mult4(pObj->mModel, rotX, tmp);
	dup4(tmp, pObj->mModel);
	mult4(pObj->mModel, rotY, tmp);
	dup4(tmp, pObj->mModel);
	mult4(pObj->mModel, transT, tmp);
}