import VectorPrototype from "./vectorprototype.js";
import { factory as Vector3Factory } from "./vector3.js";

/*
Float32Array
 */
export const factory = function(in_w, in_x, in_y, in_z, in_baseArrayClass){
	const data = new in_baseArrayClass([in_w, in_x, in_y, in_z]);
	const result = Object.create({
		"getW" : function(){
			return data[0];
		},
		"getX" : function(){
			return data[1];
		},
		"getY" : function(){
			return data[2];
		},
		"getZ" : function(){
			return data[3];
		},
		"setW" : function(in_value){
			data[0] = in_value;
			return;
		},
		"setX" : function(in_value){
			data[1] = in_value;
			return;
		},
		"setY" : function(in_value){
			data[2] = in_value;
			return;
		},
		"setZ" : function(in_value){
			data[3] = in_value;
			return;
		},
		"getRaw" : function(){
			return data;
		},
		"getRawClass" : function(){
			return in_baseArrayClass;
		}
	});

	Object.assign(result, VectorPrototype);

	return result;
}

export const factoryFloat32 = function(in_wOrUndefined, in_xOrUndefined, in_yOrUndefined, in_zOrUndefined){
	const w = (undefined === in_wOrUndefined) ? 0.0 : in_wOrUndefined;
	const x = (undefined === in_xOrUndefined) ? 0.0 : in_xOrUndefined;
	const y = (undefined === in_yOrUndefined) ? 0.0 : in_yOrUndefined;
	const z = (undefined === in_zOrUndefined) ? 0.0 : in_zOrUndefined;
	return factory(w, x, y, z, Float32Array);
}

export const factoryIdentity = function(){
	return factory(1.0, 0, 0, 0, Float32Array);
}
export const factoryYawPitchRoll = function(in_yaw, in_pitch, in_roll){
	const cy = Math.cos(in_yaw * 0.5);
	const sy = Math.sin(in_yaw * 0.5);
	const cp = Math.cos(in_pitch * 0.5);
	const sp = Math.sin(in_pitch * 0.5);
	const cr = Math.cos(in_roll * 0.5);
	const sr = Math.sin(in_roll * 0.5);

	const w = cy * cp * cr + sy * sp * sr;
	const x = cy * cp * sr - sy * sp * cr;
	const y = sy * cp * sr + cy * sp * cr;
	const z = sy * cp * cr - cy * sp * sr;

	return factory(w, x, y, z, Float32Array);
}

export const factoryMatrix33 = function(in_matrix33){
	const m00 = in_matrix33.get00();
	const m10 = in_matrix33.get10();
	const m20 = in_matrix33.get20();
	const m01 = in_matrix33.get01();
	const m11 = in_matrix33.get11();
	const m21 = in_matrix33.get21();
	const m02 = in_matrix33.get02();
	const m12 = in_matrix33.get12();
	const m22 = in_matrix33.get22();

	const w = Math.sqrt( Math.max( 0, 1 + m00 + m11 + m22 ) ) / 2;
	var x = Math.sqrt( Math.max( 0, 1 + m00 - m11 - m22 ) ) / 2;
	var y = Math.sqrt( Math.max( 0, 1 - m00 + m11 - m22 ) ) / 2;
	var z = Math.sqrt( Math.max( 0, 1 - m00 - m11 + m22 ) ) / 2;
	x = Math.abs(x) * Math.sign( m21 - m12 );
	y = Math.abs(y) * Math.sign( m02 - m20 );
	z = Math.abs(z) * Math.sign( m10 - m01 );

	return factory(w, x, y, z, Float32Array);
}

export const quaternionToYawPitchRoll = function(in_quaternion){
	const w = in_quaternion.getW();
	const x = in_quaternion.getX();
	const y = in_quaternion.getY();
	const z = in_quaternion.getZ();

	// roll (x-axis rotation)
	const sinr_cosp = +2.0 * (w * x + y * z);
	const cosr_cosp = +1.0 - 2.0 * (x * x + y * y);
	const roll = Math.atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	const sinp = +2.0 * (w * y - z * x);
	var pitch;
	if (Math.abs(sinp) >= 1){
		pitch = Math.PI / 2.0 * Math.sign(sinp); // use 90 degrees if out of range
	} else {
		pitch = Math.asin(sinp);
	}

	// yaw (z-axis rotation)
	const siny_cosp = +2.0 * (w * z + x * y);
	const cosy_cosp = +1.0 - 2.0 * (y * y + z * z);  
	const yaw = Math.atan2(siny_cosp, cosy_cosp);
	return Vector3Factory(yaw, pitch, roll, Float32Array);
}

export const factoryAxisAngle = function(in_axis, in_angle){
	const sinAngle = Math.sin(in_angle * 0.5);
	const cosAngle = Math.cos(in_angle * 0.5);
	const x = in_axis.getX() * sinAngle;
	const y = in_axis.getY() * sinAngle;
	const z = in_axis.getZ() * sinAngle;
	const w = cosAngle;
	return factory(w, x, y, z, Float32Array);
}

export const multiplication = function(in_q1, in_q2) {
	const q1w = in_q1.getW();
	const q1x = in_q1.getX();
	const q1y = in_q1.getY();
	const q1z = in_q1.getZ();
	const q2w = in_q2.getW();
	const q2x = in_q2.getX();
	const q2y = in_q2.getY();
	const q2z = in_q2.getZ();

	const x =  q1x * q2w + q1y * q2z - q1z * q2y + q1w * q2x;
	const y = -q1x * q2z + q1y * q2w + q1z * q2x + q1w * q2y;
	const z =  q1x * q2y - q1y * q2x + q1z * q2w + q1w * q2z;
	const w = -q1x * q2x - q1y * q2y - q1z * q2z + q1w * q2w;
	return factory(w, x, y, z, Float32Array);
}
