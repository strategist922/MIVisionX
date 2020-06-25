/*
Copyright (c) 2019 - 2020 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "internal_publishKernels.h"

struct OcclusionbatchPDROIDLocalData { 
	RPPCommonHandle handle;
	rppHandle_t rppHandle;
	Rpp32u device_type; 
	Rpp32u nbatchSize; 
 	RppiROI *roiPoints;
	RppiSize *srcDimensions;
	RppiSize maxSrcDimensions;
	RppiSize *dstDimensions;
	RppiSize maxDstDimensions;
	RppPtr_t pSrc1;
	RppPtr_t pSrc2;
	RppPtr_t pDst;
	vx_uint32 *src1x1;
	vx_uint32 *src1y1;
	vx_uint32 *src1x2;
	vx_uint32 *src1y2;
	vx_uint32 *src2x1;
	vx_uint32 *src2y1;
	vx_uint32 *src2x2;
	vx_uint32 *src2y2;
#if ENABLE_OPENCL
	cl_mem cl_pSrc1;
	cl_mem cl_pSrc2;
	cl_mem cl_pDst;
#endif 
};

static vx_status VX_CALLBACK refreshOcclusionbatchPDROID(vx_node node, const vx_reference *parameters, vx_uint32 num, OcclusionbatchPDROIDLocalData *data)
{
	vx_status status = VX_SUCCESS;
 	size_t arr_size;
	vx_status copy_status;
		STATUS_ERROR_CHECK(vxQueryArray((vx_array)parameters[7], VX_ARRAY_ATTRIBUTE_NUMITEMS, &arr_size, sizeof(arr_size)));
	data->src1x1 = (vx_uint32 *)malloc(sizeof(vx_uint32) * arr_size);
	copy_status = vxCopyArrayRange((vx_array)parameters[7], 0, arr_size, sizeof(vx_uint32),data->src1x1, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
		STATUS_ERROR_CHECK(vxQueryArray((vx_array)parameters[8], VX_ARRAY_ATTRIBUTE_NUMITEMS, &arr_size, sizeof(arr_size)));
	data->src1y1 = (vx_uint32 *)malloc(sizeof(vx_uint32) * arr_size);
	copy_status = vxCopyArrayRange((vx_array)parameters[8], 0, arr_size, sizeof(vx_uint32),data->src1y1, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
		STATUS_ERROR_CHECK(vxQueryArray((vx_array)parameters[9], VX_ARRAY_ATTRIBUTE_NUMITEMS, &arr_size, sizeof(arr_size)));
	data->src1x2 = (vx_uint32 *)malloc(sizeof(vx_uint32) * arr_size);
	copy_status = vxCopyArrayRange((vx_array)parameters[9], 0, arr_size, sizeof(vx_uint32),data->src1x2, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
		STATUS_ERROR_CHECK(vxQueryArray((vx_array)parameters[10], VX_ARRAY_ATTRIBUTE_NUMITEMS, &arr_size, sizeof(arr_size)));
	data->src1y2 = (vx_uint32 *)malloc(sizeof(vx_uint32) * arr_size);
	copy_status = vxCopyArrayRange((vx_array)parameters[10], 0, arr_size, sizeof(vx_uint32),data->src1y2, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
		STATUS_ERROR_CHECK(vxQueryArray((vx_array)parameters[11], VX_ARRAY_ATTRIBUTE_NUMITEMS, &arr_size, sizeof(arr_size)));
	data->src2x1 = (vx_uint32 *)malloc(sizeof(vx_uint32) * arr_size);
	copy_status = vxCopyArrayRange((vx_array)parameters[11], 0, arr_size, sizeof(vx_uint32),data->src2x1, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
		STATUS_ERROR_CHECK(vxQueryArray((vx_array)parameters[12], VX_ARRAY_ATTRIBUTE_NUMITEMS, &arr_size, sizeof(arr_size)));
	data->src2y1 = (vx_uint32 *)malloc(sizeof(vx_uint32) * arr_size);
	copy_status = vxCopyArrayRange((vx_array)parameters[12], 0, arr_size, sizeof(vx_uint32),data->src2y1, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
		STATUS_ERROR_CHECK(vxQueryArray((vx_array)parameters[13], VX_ARRAY_ATTRIBUTE_NUMITEMS, &arr_size, sizeof(arr_size)));
	data->src2x2 = (vx_uint32 *)malloc(sizeof(vx_uint32) * arr_size);
	copy_status = vxCopyArrayRange((vx_array)parameters[13], 0, arr_size, sizeof(vx_uint32),data->src2x2, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
		STATUS_ERROR_CHECK(vxQueryArray((vx_array)parameters[14], VX_ARRAY_ATTRIBUTE_NUMITEMS, &arr_size, sizeof(arr_size)));
	data->src2y2 = (vx_uint32 *)malloc(sizeof(vx_uint32) * arr_size);
	copy_status = vxCopyArrayRange((vx_array)parameters[14], 0, arr_size, sizeof(vx_uint32),data->src2y2, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
	STATUS_ERROR_CHECK(vxReadScalarValue((vx_scalar)parameters[15], &data->nbatchSize));
	STATUS_ERROR_CHECK(vxReadScalarValue((vx_scalar)parameters[20], &data->nbatchSize));
	STATUS_ERROR_CHECK(vxQueryImage((vx_image)parameters[0], VX_IMAGE_HEIGHT, &data->maxSrcDimensions.height, sizeof(data->maxSrcDimensions.height)));
	STATUS_ERROR_CHECK(vxQueryImage((vx_image)parameters[0], VX_IMAGE_WIDTH, &data->maxSrcDimensions.width, sizeof(data->maxSrcDimensions.width)));
	data->maxSrcDimensions.height = data->maxSrcDimensions.height / data->nbatchSize;
	STATUS_ERROR_CHECK(vxQueryImage((vx_image)parameters[1], VX_IMAGE_HEIGHT, &data->maxSrcDimensions.height, sizeof(data->maxSrcDimensions.height)));
	STATUS_ERROR_CHECK(vxQueryImage((vx_image)parameters[1], VX_IMAGE_WIDTH, &data->maxDstDimensions.width, sizeof(data->maxDstDimensions.width)));
	data->maxDstDimensions.height = data->maxDstDimensions.height / data->nbatchSize;
	data->srcDimensions = (RppiSize *)malloc(sizeof(RppiSize) * data->nbatchSize);
	data->dstDimensions = (RppiSize *)malloc(sizeof(RppiSize) * data->nbatchSize);
	Rpp32u *srcBatch_width = (Rpp32u *)malloc(sizeof(Rpp32u) * data->nbatchSize);
	Rpp32u *srcBatch_height = (Rpp32u *)malloc(sizeof(Rpp32u) * data->nbatchSize);
	Rpp32u *dstBatch_width = (Rpp32u *)malloc(sizeof(Rpp32u) * data->nbatchSize);
	Rpp32u *dstBatch_height = (Rpp32u *)malloc(sizeof(Rpp32u) * data->nbatchSize);
	copy_status = vxCopyArrayRange((vx_array)parameters[2], 0, data->nbatchSize, sizeof(Rpp32u),srcBatch_width, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
	copy_status = vxCopyArrayRange((vx_array)parameters[3], 0, data->nbatchSize, sizeof(Rpp32u),srcBatch_height, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
	for(int i = 0; i < data->nbatchSize; i++){
		data->srcDimensions[i].width = srcBatch_width[i];
		data->srcDimensions[i].height = srcBatch_height[i];
		data->dstDimensions[i].width = dstBatch_width[i];
		data->dstDimensions[i].height = dstBatch_height[i];
	}
	data->roiPoints = (RppiROI *)malloc(sizeof(RppiROI) * data->nbatchSize);
	Rpp32u *batch_roiX = (Rpp32u *)malloc(sizeof(Rpp32u) * data->nbatchSize);
	Rpp32u *batch_roiY = (Rpp32u *)malloc(sizeof(Rpp32u) * data->nbatchSize);
	Rpp32u *batch_roiWidth = (Rpp32u *)malloc(sizeof(Rpp32u) * data->nbatchSize);
	Rpp32u *batch_roiHeight = (Rpp32u *)malloc(sizeof(Rpp32u) * data->nbatchSize);
	copy_status = vxCopyArrayRange((vx_array)parameters[16], 0, data->nbatchSize, sizeof(Rpp32u),batch_roiX, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
	copy_status = vxCopyArrayRange((vx_array)parameters[17], 0, data->nbatchSize, sizeof(Rpp32u),batch_roiY, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
	copy_status = vxCopyArrayRange((vx_array)parameters[18], 0, data->nbatchSize, sizeof(Rpp32u),batch_roiWidth, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
	copy_status = vxCopyArrayRange((vx_array)parameters[19], 0, data->nbatchSize, sizeof(Rpp32u),batch_roiHeight, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
	for(int i = 0; i < data->nbatchSize; i++){
		data->roiPoints[i].x = batch_roiX[i];
		data->roiPoints[i].y = batch_roiY[i];
		data->roiPoints[i].roiWidth = batch_roiWidth[i];
		data->roiPoints[i].roiHeight = batch_roiHeight[i];
	}
	if(data->device_type == AGO_TARGET_AFFINITY_GPU) {
#if ENABLE_OPENCL
		STATUS_ERROR_CHECK(vxQueryImage((vx_image)parameters[0], VX_IMAGE_ATTRIBUTE_AMD_OPENCL_BUFFER, &data->cl_pSrc1, sizeof(data->cl_pSrc1)));
		STATUS_ERROR_CHECK(vxQueryImage((vx_image)parameters[1], VX_IMAGE_ATTRIBUTE_AMD_OPENCL_BUFFER, &data->cl_pSrc2, sizeof(data->cl_pSrc2)));
		STATUS_ERROR_CHECK(vxQueryImage((vx_image)parameters[4], VX_IMAGE_ATTRIBUTE_AMD_OPENCL_BUFFER, &data->cl_pDst, sizeof(data->cl_pDst)));
#endif
	}
	if(data->device_type == AGO_TARGET_AFFINITY_CPU) {
		STATUS_ERROR_CHECK(vxQueryImage((vx_image)parameters[0], VX_IMAGE_ATTRIBUTE_AMD_HOST_BUFFER, &data->pSrc1, sizeof(vx_uint8)));
		STATUS_ERROR_CHECK(vxQueryImage((vx_image)parameters[1], VX_IMAGE_ATTRIBUTE_AMD_HOST_BUFFER, &data->pSrc2, sizeof(vx_uint8)));
		STATUS_ERROR_CHECK(vxQueryImage((vx_image)parameters[4], VX_IMAGE_ATTRIBUTE_AMD_HOST_BUFFER, &data->pDst, sizeof(vx_uint8)));
	}
	return status; 
}

static vx_status VX_CALLBACK validateOcclusionbatchPDROID(vx_node node, const vx_reference parameters[], vx_uint32 num, vx_meta_format metas[])
{
	vx_status status = VX_SUCCESS;
	vx_enum scalar_type;
	STATUS_ERROR_CHECK(vxQueryScalar((vx_scalar)parameters[15], VX_SCALAR_TYPE, &scalar_type, sizeof(scalar_type)));
 	if(scalar_type != VX_TYPE_UINT32) return ERRMSG(VX_ERROR_INVALID_TYPE, "validate: Paramter: #15 type=%d (must be size)\n", scalar_type);
	STATUS_ERROR_CHECK(vxQueryScalar((vx_scalar)parameters[20], VX_SCALAR_TYPE, &scalar_type, sizeof(scalar_type)));
 	if(scalar_type != VX_TYPE_UINT32) return ERRMSG(VX_ERROR_INVALID_TYPE, "validate: Paramter: #20 type=%d (must be size)\n", scalar_type);
	STATUS_ERROR_CHECK(vxQueryScalar((vx_scalar)parameters[21], VX_SCALAR_TYPE, &scalar_type, sizeof(scalar_type)));
 	if(scalar_type != VX_TYPE_UINT32) return ERRMSG(VX_ERROR_INVALID_TYPE, "validate: Paramter: #21 type=%d (must be size)\n", scalar_type);
	// Check for input parameters 
	vx_parameter input_param; 
	vx_image input; 
	vx_df_image df_image;
	input_param = vxGetParameterByIndex(node,0);
	STATUS_ERROR_CHECK(vxQueryParameter(input_param, VX_PARAMETER_ATTRIBUTE_REF, &input, sizeof(vx_image)));
	STATUS_ERROR_CHECK(vxQueryImage(input, VX_IMAGE_ATTRIBUTE_FORMAT, &df_image, sizeof(df_image))); 
	if(df_image != VX_DF_IMAGE_U8 && df_image != VX_DF_IMAGE_RGB) 
	{
		return ERRMSG(VX_ERROR_INVALID_FORMAT, "validate: OcclusionbatchPDROID: image: #0 format=%4.4s (must be RGB2 or U008)\n", (char *)&df_image);
	}

	input_param = vxGetParameterByIndex(node,1);
	STATUS_ERROR_CHECK(vxQueryParameter(input_param, VX_PARAMETER_ATTRIBUTE_REF, &input, sizeof(vx_image)));
	STATUS_ERROR_CHECK(vxQueryImage(input, VX_IMAGE_ATTRIBUTE_FORMAT, &df_image, sizeof(df_image))); 
	if(df_image != VX_DF_IMAGE_U8 && df_image != VX_DF_IMAGE_RGB) 
	{
		return ERRMSG(VX_ERROR_INVALID_FORMAT, "validate: OcclusionbatchPDROID: image: #1 format=%4.4s (must be RGB2 or U008)\n", (char *)&df_image);
	}

	// Check for output parameters 
	vx_image output; 
	vx_df_image format; 
	vx_parameter output_param; 
	vx_uint32  height, width; 
	output_param = vxGetParameterByIndex(node,4);
	STATUS_ERROR_CHECK(vxQueryParameter(output_param, VX_PARAMETER_ATTRIBUTE_REF, &output, sizeof(vx_image))); 
	STATUS_ERROR_CHECK(vxQueryImage(output, VX_IMAGE_ATTRIBUTE_WIDTH, &width, sizeof(width))); 
	STATUS_ERROR_CHECK(vxQueryImage(output, VX_IMAGE_ATTRIBUTE_HEIGHT, &height, sizeof(height))); 
	STATUS_ERROR_CHECK(vxSetMetaFormatAttribute(metas[4], VX_IMAGE_ATTRIBUTE_WIDTH, &width, sizeof(width)));
	STATUS_ERROR_CHECK(vxSetMetaFormatAttribute(metas[4], VX_IMAGE_ATTRIBUTE_HEIGHT, &height, sizeof(height)));
	STATUS_ERROR_CHECK(vxSetMetaFormatAttribute(metas[4], VX_IMAGE_ATTRIBUTE_FORMAT, &df_image, sizeof(df_image)));
	vxReleaseImage(&input);
	vxReleaseImage(&output);
	vxReleaseParameter(&output_param);
	vxReleaseParameter(&input_param);
	return status;
}

static vx_status VX_CALLBACK processOcclusionbatchPDROID(vx_node node, const vx_reference * parameters, vx_uint32 num) 
{ 
	RppStatus status = RPP_SUCCESS;
	OcclusionbatchPDROIDLocalData * data = NULL;
	STATUS_ERROR_CHECK(vxQueryNode(node, VX_NODE_LOCAL_DATA_PTR, &data, sizeof(data)));
	vx_df_image df_image = VX_DF_IMAGE_VIRT;
	STATUS_ERROR_CHECK(vxQueryImage((vx_image)parameters[0], VX_IMAGE_ATTRIBUTE_FORMAT, &df_image, sizeof(df_image)));
	if(data->device_type == AGO_TARGET_AFFINITY_GPU) {
#if ENABLE_OPENCL
		cl_command_queue handle = data->handle.cmdq;
		refreshOcclusionbatchPDROID(node, parameters, num, data);
		if (df_image == VX_DF_IMAGE_U8 ){ 
 			// status = rppi_occlusion_u8_pln1_batchPD_ROID_gpu((void *)data->cl_pSrc1,(void *)data->cl_pSrc2,data->srcDimensions,data->maxSrcDimensions,(void *)data->cl_pDst,data->dstDimensions,data->maxDstDimensions,data->src1x1,data->src1y1,data->src1x2,data->src1y2,data->src2x1,data->src2y1,data->src2x2,data->src2y2,data->roiPoints,data->nbatchSize,data->roiPoints,data->nbatchSize,data->rppHandle);
		}
		else if(df_image == VX_DF_IMAGE_RGB) {
			// status = rppi_occlusion_u8_pkd3_batchPD_ROID_gpu((void *)data->cl_pSrc1,(void *)data->cl_pSrc2,data->srcDimensions,data->maxSrcDimensions,(void *)data->cl_pDst,data->dstDimensions,data->maxDstDimensions,data->src1x1,data->src1y1,data->src1x2,data->src1y2,data->src2x1,data->src2y1,data->src2x2,data->src2y2,data->roiPoints,data->nbatchSize,data->roiPoints,data->nbatchSize,data->rppHandle);
		}
		return status;
#endif
	}
	if(data->device_type == AGO_TARGET_AFFINITY_CPU) {
		refreshOcclusionbatchPDROID(node, parameters, num, data);
		if (df_image == VX_DF_IMAGE_U8 ){
			// status = rppi_occlusion_u8_pln1_batchPD_ROID_host(data->pSrc1,data->pSrc2,data->srcDimensions,data->maxSrcDimensions,data->pDst,data->dstDimensions,data->maxDstDimensions,data->src1x1,data->src1y1,data->src1x2,data->src1y2,data->src2x1,data->src2y1,data->src2x2,data->src2y2,data->roiPoints,data->nbatchSize,data->roiPoints,data->nbatchSize,data->rppHandle);
		}
		else if(df_image == VX_DF_IMAGE_RGB) {
			// status = rppi_occlusion_u8_pkd3_batchPD_ROID_host(data->pSrc1,data->pSrc2,data->srcDimensions,data->maxSrcDimensions,data->pDst,data->dstDimensions,data->maxDstDimensions,data->src1x1,data->src1y1,data->src1x2,data->src1y2,data->src2x1,data->src2y1,data->src2x2,data->src2y2,data->roiPoints,data->nbatchSize,data->roiPoints,data->nbatchSize,data->rppHandle);
		}
		return status;
	}
}

static vx_status VX_CALLBACK initializeOcclusionbatchPDROID(vx_node node, const vx_reference *parameters, vx_uint32 num) 
{
	OcclusionbatchPDROIDLocalData * data = new OcclusionbatchPDROIDLocalData;
	memset(data, 0, sizeof(*data));
#if ENABLE_OPENCL
	STATUS_ERROR_CHECK(vxQueryNode(node, VX_NODE_ATTRIBUTE_AMD_OPENCL_COMMAND_QUEUE, &data->handle.cmdq, sizeof(data->handle.cmdq)));
#endif
	STATUS_ERROR_CHECK(vxCopyScalar((vx_scalar)parameters[21], &data->device_type, VX_READ_ONLY, VX_MEMORY_TYPE_HOST));
	refreshOcclusionbatchPDROID(node, parameters, num, data);
#if ENABLE_OPENCL
	if(data->device_type == AGO_TARGET_AFFINITY_GPU)
		rppCreateWithStreamAndBatchSize(&data->rppHandle, data->handle.cmdq, data->nbatchSize);
#endif
	if(data->device_type == AGO_TARGET_AFFINITY_CPU)
		rppCreateWithBatchSize(&data->rppHandle, data->nbatchSize);

	STATUS_ERROR_CHECK(vxSetNodeAttribute(node, VX_NODE_LOCAL_DATA_PTR, &data, sizeof(data)));
	return VX_SUCCESS;
}

static vx_status VX_CALLBACK uninitializeOcclusionbatchPDROID(vx_node node, const vx_reference *parameters, vx_uint32 num)
{
	OcclusionbatchPDROIDLocalData * data; 
	STATUS_ERROR_CHECK(vxQueryNode(node, VX_NODE_LOCAL_DATA_PTR, &data, sizeof(data)));
#if ENABLE_OPENCL
	if(data->device_type == AGO_TARGET_AFFINITY_GPU)
		rppDestroyGPU(data->rppHandle);
#endif
	if(data->device_type == AGO_TARGET_AFFINITY_CPU)
		rppDestroyHost(data->rppHandle);
	delete(data);
	return VX_SUCCESS; 
}

vx_status OcclusionbatchPDROID_Register(vx_context context)
{
	vx_status status = VX_SUCCESS;
	// Add kernel to the context with callbacks
	vx_kernel kernel = vxAddUserKernel(context, "org.rpp.OcclusionbatchPDROID",
		VX_KERNEL_RPP_OCCLUSIONBATCHPDROID,
		processOcclusionbatchPDROID,
		22,
		validateOcclusionbatchPDROID,
		initializeOcclusionbatchPDROID,
		uninitializeOcclusionbatchPDROID);
	ERROR_CHECK_OBJECT(kernel);
	AgoTargetAffinityInfo affinity;
	vxQueryContext(context, VX_CONTEXT_ATTRIBUTE_AMD_AFFINITY,&affinity, sizeof(affinity));
#if ENABLE_OPENCL
	// enable OpenCL buffer access since the kernel_f callback uses OpenCL buffers instead of host accessible buffers
	vx_bool enableBufferAccess = vx_true_e;
	if(affinity.device_type == AGO_TARGET_AFFINITY_GPU)
		STATUS_ERROR_CHECK(vxSetKernelAttribute(kernel, VX_KERNEL_ATTRIBUTE_AMD_OPENCL_BUFFER_ACCESS_ENABLE, &enableBufferAccess, sizeof(enableBufferAccess)));
#else
	vx_bool enableBufferAccess = vx_false_e;
#endif
	if (kernel)
	{
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 0, VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 1, VX_INPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 2, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 3, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 4, VX_OUTPUT, VX_TYPE_IMAGE, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 5, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 6, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 7, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 8, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 9, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 10, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 11, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 12, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 13, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 14, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 15, VX_INPUT, VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 16, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 17, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 18, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 19, VX_INPUT, VX_TYPE_ARRAY, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 20, VX_INPUT, VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxAddParameterToKernel(kernel, 21, VX_INPUT, VX_TYPE_SCALAR, VX_PARAMETER_STATE_REQUIRED));
		PARAM_ERROR_CHECK(vxFinalizeKernel(kernel));
	}
	if (status != VX_SUCCESS)
	{
	exit:	vxRemoveKernel(kernel);	return VX_FAILURE; 
 	}
	return status;
}