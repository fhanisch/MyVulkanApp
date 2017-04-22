#include "MyVulkanEngine.h"

void initRenderObject(RenderObject *pObj, PipelineCreateInfo createInfo)
{
	pObj->pipelineCreateInfo = createInfo;
}

void motion(RenderObject *pObj, mat4 rotX, mat4 rotY, mat4 rotZ, mat4 transT)
{
	mat4 tmp;

	dup4(tmp, pObj->ubo.mView);
	mult4(pObj->ubo.mView, rotY, tmp);
	dup4(tmp, pObj->ubo.mView);
	mult4(pObj->ubo.mView, transT, tmp);
}