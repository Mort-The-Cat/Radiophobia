#ifndef RADIOPHOBIA_PARTICLE_EFFECT_DECLARATIONS
#define RADIOPHOBIA_PARTICLE_EFFECT_DECLARATIONS

#include "..\Particle_System_Declarations.h"
#include "..\OpenGL_Declarations.h"

Particle_Renderer<Smoke_Particle_Info, Billboard_Vertex_Buffer> Vent_Smoke_Particles;

class Vent_Smoke_Controller : public Controller
{
    enum Directions;

    struct Vent_Info;

    std::vector<Vent_Info> Vents;

    std::vector<float> Vent_Timers;

public:
    enum Directions
    {
        North,
        South,
        East,
        West,

        Up,
        Down
    };

    struct Vent_Info
    {
        glm::vec3 Position;
        Directions Direction;
    };

    Vent_Smoke_Controller(std::vector<Vent_Info> Ventsp)
    {
        Vents = Ventsp;

        Vent_Timers.resize(Vents.size());
    }

    virtual void Initialise_Control(Model* Objectp) override
    {
        Object = Objectp;

        for (size_t W = 0; W < Vent_Timers.size(); W++)
            Vent_Timers[W] = RNG();
    }

    virtual void Control_Function() override
    {
        for (size_t W = 0; W < Vents.size(); W++)
        {
            Vent_Timers[W] -= Tick;

            if (Vent_Timers[W] < 0.0f)
            {
                Vent_Timers[W] = RNG() * 0.4 + 0.3f;

                const float S = 1.0f;

                glm::vec3 Direction_Vectors[] =
                {
                    glm::vec3(0.0f, 0.0f, -S),
                    glm::vec3(0.0f, 0.0f, S),
                    glm::vec3(S, 0.0f, 0.0f),
                    glm::vec3(-S, 0.0f, 0.0f),

                    glm::vec3(0.0f, -S, 0.0f),
                    glm::vec3(0.0f, S, 0.0f)
                };

                glm::vec3 Delta = glm::vec3(0.25f) * glm::vec3(RNG() - 0.5f, RNG() - 0.5f, RNG() - 0.5f);

                Vent_Smoke_Particles.Particles.Spawn_Particle(Vents[W].Position + Delta, Direction_Vectors[Vents[W].Direction]);
            }
        }
    }
};

struct Shell_Particle
{
	alignas(0) glm::vec4 Position; // .w is age
	glm::vec4 Velocity; // .w is the initial X-direction of the particle

	float E; // This is the elasticity of the collision
    float Delta;
    float Q;
    float Height;
    float Height_Ratio;
    float Floor_Delta;
    float Inverse_Delta;

    float L;
    float Inverse_L; // This will be updated for every frame in the update function

    float Random_Val;
};

bool Shell_Particle_Delete_If(const Shell_Particle& Particle)
{
    return Particle.Position.w > 10.0f; // Disappears after like 5 seconds
}

class Pistol_Shell_Particle_Info : public Particle_Info<Shell_Particle>
{
private:
    float S(float X)
    {
        X = 2 * fmod(X + 10, 1) - 1;
        X *= X;
        return 1 - X;
    }

    const float N = 20.0f;

    float Inverse_G(float X, float E)
    {
        float Counter = 0.0f;
        for (float W = 0; W < N; W++)
        {
            Counter += X >= 0.0f;
            X -= powf(E, W);
        }

        return Counter;
    }

    float G(float X, float E)
    {
        float Value = 0.0f;
        for (float W = 0; W < N; W++)
        {
            Value += powf(E, W) * (X > W);
        }

        return Value;
    }

    float L(float X, float E, float Q_0, float Delta)
    {
        return Inverse_G((X - Q_0) / (Delta), E);
    }

    float Inverse_L(float X, float E, float Q_0, float Delta)
    {
        return G((X - Q_0) / (Delta), E);
    }

    float Derivative(float A, float B, float X)
    {
        return B + A * X * 2;
    }

    float Quadratic(float A, float B, float C, float X)
    {
        return C + X * (A * X + B);
    }

    void Generate_Parameters(float A, float B, float C, float E, float* Q_0, float* Q_1, float* P_0, float* Height, float* Height_Ratio)
    {
        *Q_0 = (-B + sqrtf(B * B - 4 * A * C)) / (2 * A);
        *P_0 = (-B - sqrtf(B * B - 4 * A * C)) / (2 * A);

        *Q_1 = E * (*Q_0 - *P_0) + *Q_0;

        float Middle = *Q_0 + *P_0;
        Middle *= 0.5f;

        float Middle_1 = *Q_1 + *Q_0;
        Middle_1 *= 0.5f;

        *Height = Quadratic(A, B, C, Middle);

        float Other_Height = Quadratic(
            A,
            -E * Derivative(A, B, *Q_0),
            0,
            Middle_1 - *Q_0);

        *Height_Ratio = *Height / Other_Height;
    }

    const float E = 4.0f / 6.0f;

public:

	Pistol_Shell_Particle_Info() { Particles_Per_Call = 50; }

	void Spawn_Particle(glm::vec3 Position, glm::vec4 Velocity, float Floor_Height) // This function will find out the floor position
	{
        Shell_Particle Particle;

        Particle.Random_Val = RNG();

        Particle.Velocity = Velocity;
        Particle.Position = glm::vec4(Position.x, Position.y, Position.z, 0.0f);

        Particle.Floor_Delta = Floor_Height;   // This is the distance between spawn height and floor
        Particle.Position.y -= Floor_Height;        // to make bouncing calculations easier, we use the y position relative to the floor

        float Q0, Q1, P0;

        Generate_Parameters(4.0f, Particle.Velocity.y, Particle.Position.y,
            E,
            &Q0, &Q1, &P0,
            &Particle.Height,
            &Particle.Height_Ratio);

        Particle.Q = Q0;
        Particle.Delta = Q1 - Q0;
        Particle.Inverse_Delta = 1.0f / Particle.Delta;

        Particle.E = E;

        Particles_Data.push_back(Particle);
	}

    void Update()
    {
        for (size_t W = 0; W < Particles_Data.size(); W++)
        {
            Particles_Data[W].Position.w += Tick; // .w is the time of the particle

            Particles_Data[W].L = L(Particles_Data[W].Position.w, E, Particles_Data[W].Q, Particles_Data[W].Delta);
            Particles_Data[W].Inverse_L = Inverse_L(Particles_Data[W].Position.w, E, Particles_Data[W].Q, Particles_Data[W].Delta);
        }

        auto Particles_To_Remove = std::remove_if(Particles_Data.begin(), Particles_Data.end(), Shell_Particle_Delete_If);

        Particles_Data.erase(Particles_To_Remove, Particles_Data.end());
    }
};

Particle_Renderer<Pistol_Shell_Particle_Info, Model_Vertex_Buffer> Pistol_Shell_Particles;
#endif