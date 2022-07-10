package ozz;
import h3d.shader.*;

class OzzSkinShader extends h3d.shader.SkinBase {

	static var SRC = {

		@input var input : {
			var position : Vec3;
			var normal : Vec3;
			var weights : Vec3;
			var indexes : Bytes4;
		};

		var transformedTangent : Vec4;

		var isZero: Bool;
		var isTooHigh: Bool;

		var pixelColor : Vec4;

		function vertex() {
			transformedPosition =
				(relativePosition * bonesMatrixes[int(input.indexes.x)]) * input.weights.x +
				(relativePosition * bonesMatrixes[int(input.indexes.y)]) * input.weights.y +
				(relativePosition * bonesMatrixes[int(input.indexes.z)]) * input.weights.z;
			transformedNormal =
				(input.normal * mat3(bonesMatrixes[int(input.indexes.x)])) * input.weights.x +
				(input.normal * mat3(bonesMatrixes[int(input.indexes.y)])) * input.weights.y +
				(input.normal * mat3(bonesMatrixes[int(input.indexes.z)])) * input.weights.z;

			if(fourBonesByVertex) {
				var w4 = 1 - (input.weights.x + input.weights.y + input.weights.z);
				transformedPosition += (relativePosition * bonesMatrixes[int(input.indexes.w)]) * w4;
				transformedNormal += (input.normal * mat3(bonesMatrixes[int(input.indexes.z)])) * w4;
			}

			transformedNormal = normalize(transformedNormal);

			isTooHigh = input.indexes.x > 60;
			isZero = input.indexes.x == 0;
		}

		function fragment() {
			if( isTooHigh ) pixelColor.r = 1;
			if( isZero ) pixelColor.g = 1;
		}

	};

}