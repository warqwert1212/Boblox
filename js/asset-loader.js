/**
 * Boblox Asset Loader
 * Handles loading, parsing, and integrating exported assets from the Asset Editor
 */

class BobloxAssetLoader {
    constructor() {
        this.assets = new Map();
        this.defaultAssets = {};
        this.loadedMaterials = new Map();
        this.initializeMaterials();
    }

    /**
     * Initialize material properties based on Boblox material library
     */
    initializeMaterials() {
        this.loadedMaterials.set('plastic', {
            metalness: 0.0,
            roughness: 0.7,
            color: 0xcccccc
        });
        this.loadedMaterials.set('metal', {
            metalness: 0.9,
            roughness: 0.2,
            color: 0x888888
        });
        this.loadedMaterials.set('wood', {
            metalness: 0.0,
            roughness: 0.8,
            color: 0x8b4513
        });
        this.loadedMaterials.set('glass', {
            metalness: 0.1,
            roughness: 0.1,
            color: 0xccddff,
            transparent: true,
            opacity: 0.7
        });
        this.loadedMaterials.set('gold', {
            metalness: 0.95,
            roughness: 0.1,
            color: 0xffd700
        });
        this.loadedMaterials.set('brick', {
            metalness: 0.0,
            roughness: 0.9,
            color: 0xff6b6b
        });
    }

    /**
     * Load a Boblox format asset (.boblox)
     * @param {Object} assetData - The parsed asset data
     * @param {string} assetId - Unique identifier for the asset
     */
    loadBobloxAsset(assetData, assetId) {
        const asset = {
            id: assetId,
            name: assetData.name,
            type: assetData.type,
            version: assetData.version || '1.0',
            objects: assetData.objects || [],
            metadata: {
                created: new Date().toISOString(),
                source: 'Asset Editor'
            }
        };

        this.assets.set(assetId, asset);
        return asset;
    }

    /**
     * Convert asset data to physics engine format
     * @param {Object} asset - The asset to convert
     * @returns {Array} Array of physics part objects
     */
    convertToPhysicsParts(asset) {
        return asset.objects.map((obj, index) => {
            const part = {
                id: `${asset.id}_${index}`,
                name: obj.name,
                position: {
                    x: obj.position.x || 0,
                    y: obj.position.y || 0,
                    z: obj.position.z || 0
                },
                scale: {
                    x: obj.scale.x || 1,
                    y: obj.scale.y || 1,
                    z: obj.scale.z || 1
                },
                rotation: {
                    x: obj.rotation.x || 0,
                    y: obj.rotation.y || 0,
                    z: obj.rotation.z || 0
                },
                geometry: obj.geometry,
                material: obj.material,
                properties: {
                    mass: this.calculateMass(obj),
                    friction: this.getMaterialFriction(obj.material?.name),
                    elasticity: this.getMaterialElasticity(obj.material?.name),
                    anchored: obj.type === 'static' || obj.type === 'spawn'
                }
            };

            return part;
        });
    }

    /**
     * Create default player asset
     */
    createDefaultPlayer() {
        return {
            id: 'default_player',
            name: 'Character',
            type: 'model',
            objects: [
                {
                    name: 'Torso',
                    position: { x: 0, y: 1, z: 0 },
                    scale: { x: 0.4, y: 0.6, z: 0.3 },
                    material: { name: 'plastic', color: '#4a7c0c' },
                    type: 'part'
                },
                {
                    name: 'Head',
                    position: { x: 0, y: 1.6, z: 0 },
                    scale: { x: 0.5, y: 0.5, z: 0.5 },
                    material: { name: 'plastic', color: '#ffdbac' },
                    type: 'part'
                },
                {
                    name: 'LeftArm',
                    position: { x: -0.3, y: 0.8, z: 0 },
                    scale: { x: 0.2, y: 0.8, z: 0.2 },
                    material: { name: 'plastic', color: '#4a7c0c' },
                    type: 'part'
                },
                {
                    name: 'RightArm',
                    position: { x: 0.3, y: 0.8, z: 0 },
                    scale: { x: 0.2, y: 0.8, z: 0.2 },
                    material: { name: 'plastic', color: '#4a7c0c' },
                    type: 'part'
                },
                {
                    name: 'LeftLeg',
                    position: { x: -0.15, y: 0.3, z: 0 },
                    scale: { x: 0.2, y: 0.8, z: 0.2 },
                    material: { name: 'plastic', color: '#4a7c0c' },
                    type: 'part'
                },
                {
                    name: 'RightLeg',
                    position: { x: 0.15, y: 0.3, z: 0 },
                    scale: { x: 0.2, y: 0.8, z: 0.2 },
                    material: { name: 'plastic', color: '#4a7c0c' },
                    type: 'part'
                }
            ]
        };
    }

    /**
     * Create default spawn point asset
     */
    createDefaultSpawnPoint() {
        return {
            id: 'default_spawn',
            name: 'SpawnPoint',
            type: 'part',
            objects: [
                {
                    name: 'SpawnPlate',
                    position: { x: 0, y: 0, z: 0 },
                    scale: { x: 0.5, y: 0.1, z: 0.5 },
                    material: { name: 'plastic', color: '#00ff00', emissive: '#00aa00' },
                    type: 'spawn'
                }
            ]
        };
    }

    /**
     * Create default brick asset
     */
    createDefaultBrick() {
        return {
            id: 'default_brick',
            name: 'Brick',
            type: 'part',
            objects: [
                {
                    name: 'BrickMesh',
                    position: { x: 0, y: 0, z: 0 },
                    scale: { x: 2, y: 1, z: 1 },
                    material: { name: 'brick', color: '#ff6b6b' },
                    type: 'part'
                }
            ]
        };
    }

    /**
     * Initialize all default assets
     */
    initializeDefaultAssets() {
        const player = this.createDefaultPlayer();
        const spawn = this.createDefaultSpawnPoint();
        const brick = this.createDefaultBrick();

        this.assets.set('default_player', player);
        this.assets.set('default_spawn', spawn);
        this.assets.set('default_brick', brick);

        this.defaultAssets = {
            player,
            spawn,
            brick
        };

        return this.defaultAssets;
    }

    /**
     * Get asset by ID
     */
    getAsset(assetId) {
        return this.assets.get(assetId);
    }

    /**
     * Get all assets
     */
    getAllAssets() {
        return Array.from(this.assets.values());
    }

    /**
     * Parse asset file (handles both .boblox JSON and raw JSON)
     */
    parseAssetFile(fileContent) {
        try {
            return JSON.parse(fileContent);
        } catch (e) {
            console.error('Failed to parse asset file:', e);
            return null;
        }
    }

    /**
     * Export asset to Boblox format
     */
    exportToBobloxFormat(assetId) {
        const asset = this.assets.get(assetId);
        if (!asset) return null;

        return {
            name: asset.name,
            type: asset.type,
            version: '1.0',
            objects: asset.objects,
            metadata: asset.metadata
        };
    }

    // ==================== PHYSICS HELPERS ====================

    calculateMass(obj) {
        const scale = obj.scale || { x: 1, y: 1, z: 1 };
        const volume = scale.x * scale.y * scale.z;
        
        // Base density varies by material
        const densities = {
            'plastic': 1.2,
            'wood': 0.6,
            'metal': 2.7,
            'gold': 5.0,
            'brick': 2.4,
            'glass': 1.0
        };

        const materialName = obj.material?.name || 'plastic';
        const density = densities[materialName] || 1.2;
        
        return Math.max(0.1, volume * density);
    }

    getMaterialFriction(materialName = 'plastic') {
        const friction = {
            'plastic': 0.4,
            'wood': 0.5,
            'metal': 0.3,
            'gold': 0.25,
            'brick': 0.7,
            'glass': 0.2
        };
        return friction[materialName] || 0.4;
    }

    getMaterialElasticity(materialName = 'plastic') {
        const elasticity = {
            'plastic': 0.4,
            'wood': 0.3,
            'metal': 0.5,
            'gold': 0.6,
            'brick': 0.2,
            'glass': 0.8
        };
        return elasticity[materialName] || 0.4;
    }

    /**
     * Validate asset structure
     */
    validateAsset(asset) {
        if (!asset.name || !asset.type || !asset.objects) {
            throw new Error('Invalid asset: missing required fields');
        }

        if (!Array.isArray(asset.objects)) {
            throw new Error('Invalid asset: objects must be an array');
        }

        asset.objects.forEach((obj, index) => {
            if (!obj.name || !obj.position || !obj.scale) {
                throw new Error(`Invalid object at index ${index}: missing required fields`);
            }
        });

        return true;
    }

    /**
     * Merge multiple assets into a scene
     */
    combineAssets(assetIds) {
        const combined = {
            name: 'Combined Asset',
            type: 'scene',
            objects: []
        };

        assetIds.forEach(id => {
            const asset = this.assets.get(id);
            if (asset) {
                combined.objects.push(...asset.objects);
            }
        });

        return combined;
    }

    /**
     * Transform asset's objects
     */
    transformAsset(assetId, position, rotation, scale) {
        const asset = this.assets.get(assetId);
        if (!asset) return null;

        asset.objects.forEach(obj => {
            // Apply transformations (can be additive or absolute)
            if (position) {
                obj.position.x += position.x;
                obj.position.y += position.y;
                obj.position.z += position.z;
            }
            if (scale) {
                obj.scale.x *= scale.x;
                obj.scale.y *= scale.y;
                obj.scale.z *= scale.z;
            }
        });

        return asset;
    }
}

// Export for use in both browser and Node.js
if (typeof module !== 'undefined' && module.exports) {
    module.exports = BobloxAssetLoader;
}
