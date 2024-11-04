#ifndef CREATE_GSC_SHADER_H
#define CREATE_GSC_SHADER_H

#define PS_FROM_GSC(shaderName, deviceName)                                 \
	std::make_shared<PixelShader>(PixelShader::ShaderCreateInfo{            \
		.device = deviceName,                                               \
		.shaderBlob =                                                       \
			reinterpret_cast<const void *>(gsc::shaderName::GetBytecode()), \
		.shaderBlobSize =                                                   \
			static_cast<unsigned int>(gsc::shaderName::GetBytecodeSize())   \
	})

#define VS_FROM_GSC(shaderName, deviceName)                                 \
	std::make_shared<VertexShader>(VertexShader::ShaderCreateInfo{          \
		.device = deviceName,                                               \
		.shaderBlob =                                                       \
			reinterpret_cast<const void *>(gsc::shaderName::GetBytecode()), \
		.shaderBlobSize =                                                   \
			static_cast<unsigned int>(gsc::shaderName::GetBytecodeSize())   \
	})

#define GS_FROM_GSC(shaderName, deviceName)                                 \
	std::make_shared<GeometryShader>(GeometryShader::ShaderCreateInfo{      \
		.device = deviceName,                                               \
		.shaderBlob =                                                       \
			reinterpret_cast<const void *>(gsc::shaderName::GetBytecode()), \
		.shaderBlobSize =                                                   \
			static_cast<unsigned int>(gsc::shaderName::GetBytecodeSize())   \
	})

#define CS_FROM_GSC(shaderName, deviceName)                                 \
	std::make_shared<ComputeShader>(ComputeShader::ShaderCreateInfo{        \
		.device = deviceName,                                               \
		.shaderBlob =                                                       \
			reinterpret_cast<const void *>(gsc::shaderName::GetBytecode()), \
		.shaderBlobSize =                                                   \
			static_cast<unsigned int>(gsc::shaderName::GetBytecodeSize())   \
	})

#endif	// CREATE_GSC_SHADER_H
